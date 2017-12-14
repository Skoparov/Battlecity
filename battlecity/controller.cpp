#include "controller.h"

#include <thread>

#include "ecs/systems.h"

#include <QFileInfo>

static constexpr auto map_name_pattern = ":/maps/map_%1";

namespace game
{

controller::controller( const game_settings& settings, ecs::world& world ) :
    m_world( world ),
    m_settings( settings )
{
    m_tick_timer = new QTimer{ this };
    connect( m_tick_timer, SIGNAL( timeout() ), this, SLOT( tick() ) );
}

void controller::init()
{
    load_level( m_level );
    m_need_to_load_level = false;

    // create systems
    std::unique_ptr< ecs::system > move_system{ new system::movement_system{ m_world } };

    std::unique_ptr< ecs::system > vic_def_system{
        new system::win_defeat_system{ m_settings.get_base_kills_to_win(),
                                       m_settings.get_player_lives(),
                                       m_world } };

    std::unique_ptr< ecs::system > proj_system{
        new system::projectile_system{ m_settings.get_projectile_size(),
                                       m_settings.get_projectile_damage(),
                                       m_settings.get_projectile_speed(),
                                       m_world } };

    std::unique_ptr< ecs::system > respawn_system{
        new system::respawn_system{ m_settings.get_enemies_number(), m_world } };

    m_world.add_system( *move_system );
    m_world.add_system( *vic_def_system );
    m_world.add_system( *proj_system );
    m_world.add_system( *respawn_system );

    m_systems.emplace_back( std::move( move_system ) );
    m_systems.emplace_back( std::move( vic_def_system ) );
    m_systems.emplace_back( std::move( proj_system ) );
    m_systems.emplace_back( std::move( respawn_system ) );

    m_world.subscribe< event::level_completed >( *this );
}

bool controller::load_level( uint32_t level )
{
    QString map_path{ QString{ map_name_pattern }.arg( level ) };
    QFileInfo map_file{ map_path };
    bool map_valid{ map_file.exists() && map_file.isFile() };

    if( map_valid )
    {
        read_map_file( m_map_data,
                       QString{ map_name_pattern }.arg( level ),
                       m_settings,
                       m_world,
                       m_mediator );
    }

    return map_valid;
}

void controller::start()
{
    m_tick_timer->start( 1000 / m_settings.get_fps() );
    if( m_mediator )
    {
        m_mediator->level_started( m_level );
    }
}

void controller::stop()
{
    m_tick_timer->stop();
}

void controller::set_map_mediator( map_data_mediator* mediator ) noexcept
{
    m_mediator = mediator;
}

int controller::get_rows_count() const noexcept
{
    return m_map_data.get_rows_count();
}

int controller::get_columns_count() const noexcept
{
    return m_map_data.get_columns_count();
}

int controller::get_tile_width() const noexcept
{
    return m_settings.get_tile_size().width();
}

int controller::get_tile_height() const noexcept
{
    return m_settings.get_tile_size().height();
}

uint32_t controller::get_level() const noexcept
{
    return m_level;
}

void controller::on_event( const event::level_completed& event )
{
    // TODO: check if level is present
     m_need_to_load_level = true;
    if( event.get_result() == level_game_result::victory )
    {
        ++m_level;
    }

    if( m_mediator )
    {
        m_mediator->level_ended( event.get_result() );
    }
}

void controller::tick()
{
    if( m_need_to_load_level )
    {
        std::this_thread::sleep_for( std::chrono::seconds{ 2 } );

        if( m_mediator )
        {
            m_mediator->remove_all();
        }

        m_world.reset();
        if( load_level( m_level ) )
        {
            if( m_mediator )
            {
                m_mediator->level_started( m_level );
            }

            m_need_to_load_level = false;
        }
        else // all maps have been beaten
        {
            if( m_mediator )
            {
                m_mediator->game_ended( level_game_result::victory );
                stop();
                return;
            }
        }
    }

    m_world.tick();
}

}// game
