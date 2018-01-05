#include "map_data.h"

#include <QFile>
#include <QTextStream>

#include "ecs/framework/world.h"
#include "ecs/entity_factory.h"
#include "ecs/components.h"
#include "game_settings.h"

static constexpr auto tile_char_empty = 'e';
static constexpr auto tile_char_wall = 'w';
static constexpr auto tile_char_iron_wall = 'i';
static constexpr auto tile_char_player_base = 'b';
static constexpr auto tile_char_player_start_position = 'p';

template< typename T >
T abs_diff( T l, T r ) noexcept
{
    return  l >= r? l - r : r - l;
}

namespace game
{

void map_data::set_map_size( const QSize& size ) noexcept
{
    m_map_size = size;
}

void map_data::set_map_name( const QString& name )
{
    m_map_name = name;
}

int map_data::get_rows_count() const noexcept
{
    return m_map_size.width();
}

int map_data::get_columns_count() const noexcept
{
    return m_map_size.height();
}

const QSize& map_data::get_map_size() const noexcept
{
    return m_map_size;
}

const QString &map_data::get_map_name() const noexcept
{
    return m_map_name;
}

std::pair< tile_type, object_type > char_to_tile_info( char c )
{
    tile_type ground;
    object_type obj_located_on_ground;

    switch( c )
    {
    case tile_char_empty :
        ground = tile_type::empty;
        obj_located_on_ground = object_type::tile;
        break;
    case tile_char_wall :
        ground = tile_type::wall;
        obj_located_on_ground = object_type::tile;
        break;
    case tile_char_iron_wall :
        ground = tile_type::iron_wall;
        obj_located_on_ground = object_type::tile;
        break;
    case tile_char_player_base :
        ground = tile_type::empty;
        obj_located_on_ground = object_type::player_base;
        break;
    case tile_char_player_start_position :
        ground = tile_type::empty;
        obj_located_on_ground = object_type::player_tank;
        break;
    default: throw std::invalid_argument{ std::string{ "Unknown map character: " } + c }; break;
    }

    return { ground, obj_located_on_ground };
}

QRect obj_rect( int row, int col, const QSize& tile_size, const QSize& size ) noexcept
{
    int delta_x{ ( abs_diff( size.width(), tile_size.width() ) % tile_size.width() ) / 2 };
    int delta_y{ ( abs_diff( size.height(), tile_size.height() ) % tile_size.height() ) / 2 };
    return QRect{ QPoint{ col * tile_size.width() + delta_x, row * tile_size.height() + delta_y }, size };
}

ecs::entity&
add_player_base( bool& player_base_found, int row, int col, const game_settings& settings, ecs::world& world )
{
    if( player_base_found )
    {
        throw std::logic_error{ "More than one player bases found" };
    }

    player_base_found = true;

    QRect base_rect{ obj_rect( row, col, settings.get_tile_size(), settings.get_player_base_size() ) };
    return create_entity_player_base( base_rect, settings.get_player_base_health(), world );
}

ecs::entity&
add_tank( int row, int col, const alignment& align, const game_settings& settings, ecs::world& world )
{
    QRect tank_rect{ obj_rect( row, col, settings.get_tile_size(), settings.get_tank_size() ) };

    ecs::entity& e = create_entity_tank( tank_rect,
                                         align,
                                         settings.get_tank_speed(),
                                         settings.get_tank_health(),
                                         settings.get_player_lives(),
                                         settings.get_turret_cooldown_ms(),
                                         settings.get_respawn_delay_ms(),
                                         world );

    if( align == alignment::enemy )
    {
        e.get_component< component::graphics >().set_visible( false );
        e.get_component< component::health >().decrease( settings.get_tank_health() );
    }

    return e;
}

uint32_t get_tile_health( const tile_type& type, const game_settings& settings )
{
    uint32_t health{ 0 };

    switch( type )
    {
    case tile_type::wall: health = settings.get_wall_health(); break;
    case tile_type::iron_wall: health = settings.get_iron_wall_health(); break;
    default: break;
    }

    return health;
}

ecs::entity&
add_tile( const tile_type& type, int row, int col, const game_settings& settings, ecs::world& world )
{
    const QSize& tile_size{ settings.get_tile_size() };
    return create_entity_tile( type,
                               obj_rect( row, col, tile_size, tile_size ),
                               get_tile_health( type, settings ),
                               world );
}

ecs::entity&
add_powerup( const powerup_type& type, const game_settings& settings, ecs::world& world )
{
    const QSize& tile_size{ settings.get_tile_size() };
    return create_power_up( QRect{ QPoint{}, tile_size },
                            type,
                            std::chrono::milliseconds{ settings.get_powerup_respawn_timeout( type ) },
                            world );
}

void read_map_file( map_data& data,
                    const QString& file,
                    const game_settings& settings,
                    ecs::world& world,
                    map_data_mediator* mediator )
{
    QFile map_file{ file };
    if ( !map_file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        throw std::ios_base::failure{ "Failed to open map file" };
    }

    QTextStream text_stream{ &map_file };
    if( text_stream.atEnd() )
    {
        throw std::logic_error{ "Map file is empty" };
    }

    data.set_map_name( text_stream.readLine() );

    if( text_stream.atEnd() )
    {
        throw std::logic_error{ "Map file is empty" };
    }

    int rows_count{ 0 };
    int columns_count{ 0 };

    char tile_char;
    int curr_column{ 0 };
    bool player_start_pos_found{ false };
    bool player_base_found{ false };

    map_graph& graph = data.get_map_graph();
    graph.clear();

    while( !text_stream.atEnd() )
    {
        text_stream >> tile_char;

        if( tile_char != '\n' )
        {
            std::pair< tile_type, object_type > tile_info{ char_to_tile_info( tile_char ) };
            const tile_type& type{ tile_info.first };

            ecs::entity& tile_entity = add_tile( type, rows_count, curr_column, settings, world );

            map_tile_node& node =
                    create_map_node( tile_entity, rows_count, curr_column, columns_count, graph );

            ecs::entity* entity{ nullptr };
            if( tile_info.second == object_type::player_base )
            {
                entity = &add_player_base( player_base_found, rows_count, curr_column, settings, world );
            }
            else if( tile_info.second == object_type::player_tank )
            {
                if( player_start_pos_found )
                {
                    throw std::logic_error{ "More than one player start position found" };
                }

                player_start_pos_found = true;
                entity = &add_tank( rows_count, curr_column, alignment::player, settings, world );
            }

            tile_entity.add_component< component::positioning >( node );
            if( entity )
            {
                entity->add_component< component::positioning >( node );
            }

            if( mediator )
            {
                if( entity )
                {
                    mediator->add_object( tile_info.second, entity, false );
                }

                mediator->add_object( object_type::tile, &tile_entity, false );
            }

            ++curr_column;
        }
        else
        {
            if( columns_count && columns_count != curr_column )
            {
                throw std::logic_error{ "Length of all tile rows must be equal" };
            }
            else if( !columns_count )
            {
                columns_count = curr_column;
            }

            ++rows_count;
            curr_column = 0;
        }
    }

    if( !player_base_found )
    {
        throw std::logic_error{ "Player base not found" };
    }
    else if( !player_start_pos_found )
    {
        throw std::logic_error{ "Player start position not found" };
    }

    for( uint32_t enemy{ 0 }; enemy < settings.get_enemies_number(); ++enemy )
    {
        ecs::entity& entity = add_tank( 0, 0, alignment::enemy, settings, world );
        entity.add_component< component::positioning >();
        if( mediator )
        {
            mediator->add_object( object_type::enemy_tank, &entity, false );
        }
    }

    for( uint32_t frag{ 0 }; frag < settings.get_base_kills_to_win(); ++frag )
    {
        ecs::entity& entity = create_entity_frag( QRect{ 0, 0, 32, 32 }, world, frag );
        if( mediator )
        {
            mediator->add_object( object_type::frag, &entity, false );
        }
    }

    ecs::entity& shield_entity = add_powerup( powerup_type::shield, settings, world );
    shield_entity.add_component< component::positioning >();
    if( mediator )
    {        
        mediator->add_object( object_type::power_up, &shield_entity, false );
    }

    QSize map_size{ columns_count, rows_count };
    data.set_map_size( map_size );

    // add map entity
    const QSize& tile_size{ settings.get_tile_size() };
    QRect map_rect{ 0, 0, tile_size.width() * map_size.width(), tile_size.height() * map_size.height() };
    create_map_entity( map_rect, world );
}

}// game
