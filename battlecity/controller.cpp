#include "controller.h"

#include <QFileInfo>

#include "ecs/systems.h"

static constexpr auto map_name_pattern = ":/maps/map_%1";

namespace game
{

controller::controller( const game_settings& settings, ecs::world& world ) :
    m_settings( settings ),
    m_world( world )
{
    m_tick_timer = new QTimer{ this };
    connect( m_tick_timer, SIGNAL( timeout() ), this, SLOT( tick() ) );
}

controller::~controller()
{
    m_world.subscribe< event::projectile_fired >( *this );
    m_world.subscribe< event::entities_removed >( *this );
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

    m_world.add_system( *move_system );
    m_world.add_system( *vic_def_system );
    m_world.add_system( *proj_system );

    m_systems.emplace_back( std::move( move_system ) );
    m_systems.emplace_back( std::move( vic_def_system ) );
    m_systems.emplace_back( std::move( proj_system ) );

    m_world.subscribe< event::projectile_fired >( *this );
    m_world.subscribe< event::entities_removed >( *this );
    m_world.subscribe< event::level_completed >( *this );
}

void controller::load_level( uint32_t level )
{
    read_map_file( m_map_data,
                   QString{ map_name_pattern }.arg( level ) ,
                   m_settings,
                   m_world );
}

void controller::start()
{
    m_tick_timer->start( 1000 / m_settings.get_fps() );
}

void controller::stop()
{
    m_tick_timer->stop();
    m_map_data = {};
    m_level = 0;
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

QList< tile_map_object* > controller::get_tiles() const
{
    return m_map_data.get_objects_of_type< object_type::tile >();
}

QList< graphics_map_object* > controller::get_player_bases() const
{
    return m_map_data.get_objects_of_type< object_type::player_base >();
}

QList< tank_map_object* > controller::get_player_tanks() const
{
    return m_map_data.get_objects_of_type< object_type::player_tank >();
}

QList< movable_map_object* > controller::get_projectiles() const
{
    return m_map_data.get_objects_of_type< object_type::projectile >();
}

void controller::on_event( const event::projectile_fired& event )
{
    std::unique_ptr< base_map_object > projectile{
        new movable_map_object{ &event.get_projectile(), object_type::projectile } };

    m_world.subscribe< event::geometry_changed >( *projectile );
    m_map_data.add_object( std::move( projectile ) );

    emit projectile_fired();
}

void controller::on_event( const event::entities_removed& event )
{
    auto removed_object_types = m_map_data.remove_objects_from_active( event.get_entities() );
    emit objects_removed( std::move( removed_object_types ) );
}

void controller::on_event( const event::level_completed& event )
{
    // TODO: check if level is present
     m_need_to_load_level = true;

    if( event.get_result() == level_result::victory )
    {
        ++m_level;
    }
}

void controller::tick()
{
    if( m_need_to_load_level )
    {
        m_map_data.remove_all_objects_from_active();

        emit level_updated();

        m_map_data.clear_inactive_objects();
        m_world.reset();

        load_level( m_level );
        m_need_to_load_level = false;

        emit level_updated();
    }
    else
    {
        m_map_data.clear_inactive_objects();
    }

    m_world.tick();
}

}// game
