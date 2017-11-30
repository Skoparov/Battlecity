#ifndef MAPDATA_H
#define MAPDATA_H

#include<QHash>
#include<QPoint>

enum class tile_type{ wall, empty, player_base };

inline uint qHash( const QPoint& key );

struct MapData
{
    uint32_t rows{ 0 };
    uint32_t columns{ 0 };
    QHash< QPoint, tile_type > tiles_data;
};

MapData read_map_file( const QString& file );

#endif // MAPDATA_H
