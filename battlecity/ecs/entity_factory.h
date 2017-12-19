#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <QRect>

#include "framework/world.h"
#include "general_enums.h"

namespace game
{

ecs::entity& create_explosion( const QRect& rect, ecs::world& world );
ecs::entity& create_entity_frag( const QRect& rect, ecs::world& world, uint32_t num );
ecs::entity& create_respawn_point_entity( const QRect& rect, ecs::world& world );
ecs::entity& create_map_entity( const QRect& rect, ecs::world& world );
ecs::entity& create_entity_player_base( const QRect& rect, uint32_t health, ecs::world& world );
ecs::entity& create_entity_tile( const tile_type& type, const QRect& rect, ecs::world& world );
ecs::entity& create_entity_projectile( const QRect& rect,
                                       uint32_t damage,
                                       uint32_t speed,
                                       const movement_direction& direction,
                                       ecs::entity& owner,
                                       ecs::world& world );

ecs::entity& create_entity_tank( const QRect& rect,
                                 const alignment& align,
                                 uint32_t speed,
                                 uint32_t health,
                                 uint32_t lifes,
                                 uint32_t turret_cooldown_msec,
                                 ecs::world& world );

QString tile_image_path( const tile_type& type );

}// game

#endif
