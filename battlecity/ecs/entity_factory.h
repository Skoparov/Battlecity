#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "framework/world.h"
#include "components.h"

namespace game
{

ecs::entity& create_world_entity( const QSize& tile_size,
                                  const QSize& map_size,
                                  ecs::world& world );

ecs::entity& create_entity_tile( const tile_type& type,
                                 const QPoint& pos,
                                 const QSize& size,
                                 ecs::world& world );

}// game

#endif
