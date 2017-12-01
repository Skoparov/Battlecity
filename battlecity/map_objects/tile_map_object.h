#ifndef TILE_MAP_OBJECT_H
#define TILE_MAP_OBJECT_H

#include "graphics_map_object.h"

enum class tile_type{ wall, empty, player_base };

class tile_map_object : public graphics_map_object
{
    Q_OBJECT

public:
    tile_map_object( const tile_type& type, QObject* parent = nullptr );

    const tile_type& get_tile_type() const noexcept;

    Q_PROPERTY( tile_type type READ get_tile_type CONSTANT )

private:
    const tile_type m_type{ tile_type::empty };
};

#endif
