#include "MapData.h"

#include <QFile>
#include <QTextStream>

#define TILE_EMPTY 'e'
#define TILE_WALL 'w'
#define TILE_PLAYER_BASE 'b'

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

MapData read_map_file( const QString& file )
{
    QFile map_file{ file };
    if ( !map_file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        throw std::ios_base::failure{ "Failed to open map file" };
    }

    QTextStream text_stream{ &map_file };
    MapData data;
    data.rows = text_stream.atEnd()? 0 : 1;

    char tile_char;
    uint32_t curr_row_len{ 0 };
    bool player_base_found{ false };

    while( !text_stream.atEnd() )
    {
        text_stream >> tile_char;

        if( tile_char != '\n' )
        {
            tile_type type{ char_to_tile_type( tile_char ) };

            if( type == tile_type::player_base )
            {
                if( player_base_found )
                {
                    throw std::logic_error{ "More than one player bases found" };
                }

                player_base_found = true;
            }

            data.tiles_data.insert( QPoint{ curr_row_len, data.rows - 1 }, type );
            ++curr_row_len;
        }
        else
        {
            if( data.columns && data.columns != curr_row_len )
            {
                throw std::logic_error{ "Length of all tile rows must be equal" };
            }
            else if( !data.columns )
            {
                data.columns = curr_row_len;
            }

            ++data.rows;
            curr_row_len = 0;
        }
    }

    if( !player_base_found )
    {
        throw std::logic_error{ "Player base not found" };
    }

    return data;
}

uint qHash(const QPoint &key)
{
    return qHash (static_cast <qint64> (key.x () ) << 32 | key.y () );
}
