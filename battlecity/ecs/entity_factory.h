#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include "framework/world.h"
#include "components.h"

namespace game
{

namespace factory
{

enum class entity_type{ tile, tank, projectile };

template< typename... entity_specific_args >
ecs::entity& create_entity( const entity_type& type,
                            ecs::world* world,
                            entity_specific_args&& ...args );
}// components

}// game

#endif
