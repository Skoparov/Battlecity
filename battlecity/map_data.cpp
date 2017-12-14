#include "map_data.h"

#include <QFile>
#include <QTextStream>

#include "ecs/framework/world.h"
#include "ecs/entity_factory.h"

#include "game_settings.h"

static constexpr auto tile_char_empty = 'e';
static constexpr auto tile_char_wall = 'w';
static constexpr auto tile_char_player_base = 'b';
static constexpr auto tile_char_player_start_position = 'p';

namespace game
{

void map_data::set_map_size( const QSize& size ) noexcept
{
    m_map_size = size;
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
    return QRect{ QPoint{ col * tile_size.width(), row * tile_size.height() }, size };
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
add_tank( bool& player_start_pos_found, int row, int col, const game_settings& settings, ecs::world& world )
{
    if( player_start_pos_found )
    {
        throw std::logic_error{ "More than one player start position found" };
    }

    player_start_pos_found = true;

    QRect player_tank_rect{ obj_rect( row, col, settings.get_tile_size(), settings.get_tank_size() ) };
    return create_entity_tank( player_tank_rect,
                               tank_type::player,
                               settings.get_tank_speed(),
                               settings.get_tank_health(),
                               world );
}

ecs::entity&
add_tile( const tile_type& type, int row, int col, const game_settings& settings, ecs::world& world )
{
    const QSize& tile_size{ settings.get_tile_size() };
    return create_entity_tile( type,
                        obj_rect( row, col, tile_size, tile_size ),
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

    int rows_count{ 0 };
    int columns_count{ 0 };

    char tile_char;
    int curr_column{ 0 };
    bool player_start_pos_found{ false };
    bool player_base_found{ false };

    while( !text_stream.atEnd() )
    {
        text_stream >> tile_char;

        if( tile_char != '\n' )
        {
            std::pair< tile_type, object_type > tile_info{ char_to_tile_info( tile_char ) };
            const tile_type& type{ tile_info.first };

            ecs::entity* entity{ nullptr };
            if( tile_info.second == object_type::player_base )
            {
                entity = &add_player_base( player_base_found, rows_count, curr_column, settings, world );
            }
            else if( tile_info.second == object_type::player_tank )
            {
                entity = &add_tank( player_start_pos_found, rows_count, curr_column, settings, world );
            }

            ecs::entity& tile_entity = add_tile( type, rows_count, curr_column, settings, world );

            if( mediator )
            {
                if( entity )
                {
                    mediator->add_object( tile_info.second, *entity );
                }

                mediator->add_object( object_type::tile, tile_entity );
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

    QSize map_size{ columns_count, rows_count };
    data.set_map_size( map_size );

    // add map entity
    const QSize& tile_size{ settings.get_tile_size() };
    QRect map_rect{ 0, 0, tile_size.width() * map_size.width(), tile_size.height() * map_size.height() };
    add_map_entity( map_rect, world );
}

}// game
