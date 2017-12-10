#include "entity_factory.h"

static constexpr auto image_tile_empty = "tile_empty";
static constexpr auto image_tile_wall = "tile_wall";
static constexpr auto image_player_base = "player_base";

namespace game
{

ecs::entity& create_entity_map( const QRect& rect, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::map_object >();
    entity.add_component< component::geometry >( rect );

    return entity;
}

QString get_image_path( const QString& image_name )
{
    return QString( "qrc:/graphics/%1.png" ).arg( image_name );
}

ecs::entity& create_entity_player_base( const QRect& rect, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::player_base >();
    entity.add_component< component::geometry >( rect );
    entity.add_component< component::graphics >( get_image_path( image_player_base ) );

    return entity;
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

ecs::entity& create_entity_tile( const tile_type& type, const QRect& rect, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    try
    {
        entity.add_component< component::geometry >( rect );
        entity.add_component< component::graphics >( tile_image_path( type ) );

        if( !tile_traversible( type ) )
        {
            entity.add_component< component::non_traversible >();
        }
    }
    catch( ... )
    {
        world.remove_entity( entity );
        throw;
    }

    return entity;

}

// factory

}// ecs
