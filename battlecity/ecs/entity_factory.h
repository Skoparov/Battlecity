#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "framework/world.h"
#include "components.h"

namespace game
{

ecs::entity& create_entity_tile( const tile_type& type,
                                 const QPoint& pos,
                                 const QSize& size,
                                 ecs::world& world );

}// game

#endif
