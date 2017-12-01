#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "framework/world.h"
#include "components.h"

namespace game
{

void create_tile( const tile_type& type, const QPoint& pos, const QSize& size, int rotation, ecs::world& world );

}// game

#endif
