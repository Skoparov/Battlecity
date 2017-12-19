#include "controller.h"

#include <thread>

#include "ecs/systems.h"

#include <QDir>
#include <QFileInfo>

static constexpr auto map_extension = "bsmap";
static constexpr auto map_dir_path = ":/maps/";

static const uint32_t map_switch_pause_duration{ 2000 };

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
    QDir map_dir{ map_dir_path };
    map_dir.setNameFilters( QStringList{} <<  QString{ "*.%1" }.arg( map_extension ) );
    m_levels = map_dir.entryList( QDir::NoDotAndDotDot | QDir::AllEntries );

    if( m_levels.empty() )
    {
        throw std::logic_error{ "No maps found" };
    }

    // create systems
    std::unique_ptr< ecs::system > move_system{ new system::movement_system{ m_world } };
    std::unique_ptr< ecs::system > explosion_system{ new system::explosion_system{ m_world } };

    std::unique_ptr< ecs::system > vic_def_system{
        new system::win_defeat_system{ m_settings.get_base_kills_to_win(), m_world } };

    std::unique_ptr< ecs::system > proj_system{
        new system::projectile_system{ m_settings.get_projectile_size(),
                    m_settings.get_projectile_damage(),
                    m_settings.get_projectile_speed(),
                    m_world } };

    std::unique_ptr< ecs::system > respawn_system{
        new system::respawn_system{ std::chrono::milliseconds{ m_settings.get_respawn_delay_ms() },
                                    m_world } };

    std::unique_ptr< ecs::system > tank_ai_system{
        new system::tank_ai_system{ m_settings.get_ai_chance_to_fire(), m_world } };

    load_level();
    m_need_to_load_level = false;

    m_world.add_system( *move_system );
    m_world.add_system( *explosion_system );
    m_world.add_system( *vic_def_system );
    m_world.add_system( *proj_system );
    m_world.add_system( *respawn_system );
    m_world.add_system( *tank_ai_system );

    m_systems.emplace_back( std::move( move_system ) );
    m_systems.emplace_back( std::move( explosion_system ) );
    m_systems.emplace_back( std::move( vic_def_system ) );
    m_systems.emplace_back( std::move( proj_system ) );
    m_systems.emplace_back( std::move( respawn_system ) );
    m_systems.emplace_back( std::move( tank_ai_system ) );

    m_world.subscribe< event::level_completed >( *this );
}

QString get_map_path( const QString& map_name )
{
    return QString{ map_dir_path } + map_name;
}

void controller::load_level()
{
    read_map_file( m_map_data,
                   get_map_path( m_levels.front() ),
                   m_settings,
                   m_world,
                   m_mediator );

    for( auto& system : m_systems )
    {
        system->init();
    }
}

void controller::start()
{
    m_tick_timer->start( 1000 / m_settings.get_fps() );
    if( m_mediator )
    {
        m_mediator->level_started( m_map_data.get_map_name() );
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

int controller::get_rows_num() const noexcept
{
    return m_map_data.get_rows_count();
}

int controller::get_columns_num() const noexcept
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

const QString& controller::get_level() const noexcept
{
    return m_map_data.get_map_name();
}

uint32_t controller::get_player_remaining_lifes()
{
    ecs::entity* player{ m_world.get_entities_with_component< component::player >().front() };
    return player->get_component< component::lifes >().get_lifes();
}

uint32_t controller::get_base_remaining_health()
{
    ecs::entity* player_base{ m_world.get_entities_with_component< component::player_base >().front() };
    return player_base->get_component< component::health >().get_health();
}

void controller::on_event( const event::level_completed& event )
{
    // TODO: check if level is present
    m_need_to_load_level = true;
    if( event.get_result() == level_game_result::victory )
    {
        m_levels.pop_front();
    }

    if( m_mediator )
    {
        if( !m_levels.empty() ) // all maps have been beaten
        {
            m_mediator->level_ended( event.get_result() );
        }
        else
        {
            stop();
            m_mediator->game_ended( level_game_result::victory );
        }
    }
}

void controller::tick()
{
    if( m_need_to_load_level )
    {
        m_need_to_load_level = false;

        if( !m_levels.empty() )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds{ map_switch_pause_duration } );
            if( m_mediator )
            {
                m_mediator->remove_all();
            }

            m_world.reset();
            load_level();

            if( m_mediator )
            {
                m_mediator->level_started( m_map_data.get_map_name() );
            }
        }
    }

    m_world.tick();
}

}// game
