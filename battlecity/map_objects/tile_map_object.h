#ifndef TILE_MAP_OBJECT_H
#define TILE_MAP_OBJECT_H

#include "graphics_map_object.h"
#include "ecs/components.h"

namespace game
{

class tile_map_object : public graphics_map_object
{
    Q_OBJECT

public:
    tile_map_object() = default;
    tile_map_object( ecs::entity* entity, QObject* parent = nullptr );

    const tile_type& get_tile_type() const noexcept;

    Q_PROPERTY( tile_type type READ get_tile_type CONSTANT )
};

}

#endif
