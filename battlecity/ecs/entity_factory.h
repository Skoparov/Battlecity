#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

#include <chrono>

#include <QRect>

#include "framework/world.h"
#include "general_enums.h"

namespace game
{

ecs::entity& create_animation( const QRect &rect,
                               const animation_data& params,
                               const animation_type& type,
                               ecs::world& world );

ecs::entity& create_power_up( const QRect& rect,
                              const powerup_type& type,
                              const std::chrono::milliseconds& respawn_time,
                              ecs::world& world );

ecs::entity& create_entity_frag( const QRect& rect, ecs::world& world, uint32_t num );
ecs::entity& create_map_entity( const QRect& rect, ecs::world& world );
ecs::entity& create_entity_player_base( const QRect& rect, uint32_t health, ecs::world& world );
ecs::entity& create_entity_tile( const tile_type& type, const QRect& rect, uint32_t health, ecs::world& world );

struct projectile_params
{
    QRect rect;
    uint32_t damage;
    uint32_t speed;
    movement_direction direction;
    ecs::entity& owner;
};

ecs::entity& create_entity_projectile( const projectile_params& params, ecs::world& world );

struct tank_entity_params
{
    QRect rect;
    uint32_t speed;
    uint32_t health;
    uint32_t lifes;
    std::chrono::milliseconds turret_cooldown_msec;
    std::chrono::milliseconds respawn_delay;
    alignment align;
};

ecs::entity& create_entity_tank( const tank_entity_params& params, ecs::world& world );

QString tile_image_path( const tile_type& type );

}// game

#endif
