#include "map_data.h"

#include <QFile>
#include <QTextStream>

#include "ecs/entity_factory.h"

#define TILE_EMPTY 'e'
#define TILE_WALL 'w'
#define TILE_PLAYER_BASE 'b'

#define PLAYER_BASE_HEALTH 5

namespace game
{

map_data::map_data(const QSize& map_size,
                   const QSize& tile_size,
                   std::list<std::unique_ptr<base_map_object> >&& objects ) noexcept:
    m_map_size( map_size ),
    m_tile_size( tile_size ),
    m_map_objects( std::move( objects ) ){}

int map_data::get_rows_count() const noexcept
{
    return m_map_size.width();
}

int map_data::get_columns_count() const noexcept
{
    return m_map_size.height();
}

const QSize &map_data::get_map_size() const noexcept
{
    return m_map_size;
}

int map_data::get_tile_width() const noexcept
{
    return m_tile_size.width();
}

int map_data::get_tile_height() const noexcept
{
    return m_tile_size.height();
}

const QSize& map_data::get_tile_size() const noexcept
{
    return m_tile_size;
}

tile_type char_to_tile_type( char c )
{
    tile_type type;
    switch( c )
    {
    case TILE_EMPTY : type = tile_type::empty; break;
    case TILE_WALL : type = tile_type::wall; break;
    case TILE_PLAYER_BASE : type = tile_type::player_base; break;
    default: throw std::invalid_argument{ std::string{ "Unknown map character: " } + c }; break;
    }

    return type;
}

map_data read_map_file( const QString& file, ecs::world& world )
{
    QFile map_file{ file };
    if ( !map_file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        throw std::ios_base::failure{ "Failed to open map file" };
    }

    QTextStream text_stream{ &map_file };

    QString tile_row_size{ text_stream.readLine() };
    QStringList tile_width_height{ tile_row_size.split( ' ' ) };
    if( tile_width_height.size() != 2 )
    {
        throw std::logic_error{ "Map file is corrupted" };
    }

    if( text_stream.atEnd() )
    {
        throw std::logic_error{ "Map file is empty" };
    }

    QSize tile_size{ tile_width_height.first().toInt(),
                     tile_width_height.first().toInt() };

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
            tile_type type{ char_to_tile_type( tile_char ) };

            uint32_t tile_health{ 0 };

            if( type == tile_type::player_base )
            {
                if( player_base_found )
                {
                    throw std::logic_error{ "More than one player bases found" };
                }

                player_base_found = true;
                tile_health = PLAYER_BASE_HEALTH;
            }

            ecs::entity& tile_entity =
                    create_entity_tile( type, QPoint{ rows_count, curr_column }, tile_size, world, tile_health );

            std::unique_ptr< base_map_object > tile{ new tile_map_object{ tile_entity } };
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

    return map_data{ QSize{ rows_count, columns_count }, tile_size, std::move( objects ) };
}

}// game
