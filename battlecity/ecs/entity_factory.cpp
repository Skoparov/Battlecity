#include "entity_factory.h"

#include "components.h"

static constexpr auto image_tile_empty = "tile_empty";
static constexpr auto image_tile_wall = "tile_wall";
static constexpr auto image_player_base = "player_base";
static constexpr auto image_player_tank = "player_tank";
static constexpr auto image_enemy_tank = "enemy_tank";
static constexpr auto image_projectile = "projectile";

namespace game
{

QString get_image_path( const QString& image_name )
{
    return QString( "qrc:/graphics/%1.png" ).arg( image_name );
}

bool tile_traversible( const tile_type& type )
{
    bool traversible{ false };

    switch( type )
    {
    case tile_type::empty : traversible = true; break;
    case tile_type::wall : traversible = false; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return traversible;
}

QString tile_image_path( const tile_type& type )
{
    QString image_name;

    switch( type )
    {
    case tile_type::empty : image_name = image_tile_empty; break;
    case tile_type::wall : image_name = image_tile_wall; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return get_image_path( image_name );
}

QString tank_image_path( const alignment& align )
{
    QString image_name;

    switch( align )
    {
    case alignment::player : image_name = image_player_tank; break;
    case alignment::enemy : image_name = image_enemy_tank; break;
    default: throw std::invalid_argument{ "Unimplemented tank type" };
    }

    return get_image_path( image_name );
}

ecs::entity& create_respawn_point_entity( const QRect& rect, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::respawn_point >();
    entity.add_component< component::geometry >( rect );

    return entity;
}

ecs::entity& create_map_entity( const QRect& rect, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::game_map >();
    entity.add_component< component::geometry >( rect );

    return entity;
}

ecs::entity& create_entity_player_base( const QRect& rect, uint32_t health, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::player_base >();
    entity.add_component< component::geometry >( rect );
    entity.add_component< component::health >( health );
    entity.add_component< component::non_traversible >();
    entity.add_component< component::graphics >( get_image_path( image_player_base ) );

    return entity;
}

ecs::entity& create_entity_tile( const tile_type& type, const QRect& rect, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    try
    {
        entity.add_component< component::geometry >( rect );
        entity.add_component< component::tile_object >( type );
        entity.add_component< component::graphics >( tile_image_path( type ) );

        if( !tile_traversible( type ) )
        {
            entity.add_component< component::non_traversible >();
            uint32_t health = 1;
            entity.add_component< component::health >( health ); // TODO!!!!!!!!
        }
    }
    catch( ... )
    {
        world.remove_entity( entity );
        throw;
    }

    return entity;
}

ecs::entity& create_entity_tank( const QRect& rect,
                                 const alignment& align,
                                 uint32_t speed,
                                 uint32_t health,
                                 ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    try
    {
        entity.add_component< component::tank_object >();
        entity.add_component< component::geometry >( rect );
        entity.add_component< component::health >( health );
        entity.add_component< component::non_traversible >();
        entity.add_component< component::movement >( speed );
        entity.add_component< component::graphics >( tank_image_path( align ) );

        if( align == alignment::player )
        {
            entity.add_component< component::player >();
        }
        else
        {
            entity.add_component< component::enemy >();
        }
    }
    catch( ... )
    {
        world.remove_entity( entity );
        throw;
    }

    return entity;
}

ecs::entity& create_entity_projectile( const QRect& rect,
                                       uint32_t damage,
                                       uint32_t speed,
                                       const movement_direction& direction,
                                       ecs::entity_id owner,
                                       ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::projectile >( damage, owner );
    entity.add_component< component::geometry >( rect );
    entity.add_component< component::movement >( speed, direction );
    entity.add_component< component::graphics >( get_image_path( image_projectile ) );

    return entity;
}

// factory

}// ecs
