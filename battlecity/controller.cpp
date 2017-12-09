#include "controller.h"

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

void controller::init()
{
    load_next_level();

    // create systems
    std::unique_ptr< ecs::system > move_system{ new systems::movement_system{ m_world } };
    m_world.add_system( *move_system );

    m_systems.emplace_back( std::move( move_system ) );
}

void controller::load_next_level()
{
    m_map_data = read_map_file( QString{ map_name_pattern }.arg( ++m_level ),
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

QList< tile_map_object* > controller::get_tiles()
{
    return m_map_data.get_objects_of_type< object_type::tile >();
}

void controller::tick()
{
    m_world.tick();
}

}// game
