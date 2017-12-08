#include "map_data.h"

#include <QFile>
#include <QTextStream>

#include "ecs/entity_factory.h"

#include "game_settings.h"

static constexpr auto tile_char_empty = 'e';
static constexpr auto tile_char_wall = 'w';
static constexpr auto tile_char_player_base = 'b';

namespace game
{

map_data::map_data( const QSize& map_size,
                    std::list< std::unique_ptr< base_map_object > >&& objects ) noexcept:
    m_map_size( map_size ),
    m_map_objects( std::move( objects ) ){}

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

std::pair< tile_type, bool > char_to_tile_info( char c )
{
    tile_type type;
    bool is_player_base{ false };

    switch( c )
    {
    case tile_char_empty : type = tile_type::empty; break;
    case tile_char_wall : type = tile_type::wall; break;
    case tile_char_player_base :
        type = tile_type::wall;
        is_player_base = true;
        break;
    default: throw std::invalid_argument{ std::string{ "Unknown map character: " } + c }; break;
    }

    return { type, is_player_base };
}

map_data read_map_file( const QString& file, const game_settings& settings, ecs::world& world )
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
    bool player_base_found{ false };
    std::list< std::unique_ptr< base_map_object > > objects;

    while( !text_stream.atEnd() )
    {
        text_stream >> tile_char;

        if( tile_char != '\n' )
        {
            std::pair< tile_type, bool > tile_info{ char_to_tile_info( tile_char ) };
            const tile_type& type{ tile_info.first };

            if( tile_info.second ) // is player base?
            {
                if( player_base_found )
                {
                    throw std::logic_error{ "More than one player bases found" };
                }

                player_base_found = true;                
            }

            ecs::entity& tile_entity = create_entity_tile( type,
                                                           QPoint{ rows_count, curr_column },
                                                           settings.get_tile_size(),
                                                           world );

            std::unique_ptr< base_map_object > tile{ new tile_map_object{ &tile_entity } };
            objects.emplace_back( std::move( tile ) );
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

    return map_data{ QSize{ columns_count, rows_count }, std::move( objects ) };
}

}// game
