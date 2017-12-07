#include "entity_factory.h"

#define TILE_IMAGE_PATH_EMPTY "tile_empty"
#define TILE_IMAGE_PATH_WALL "tile_wall"
#define TILE_IMAGE_PATH_PLAYER_BASE "tile_player_base"

namespace game
{

bool tile_traversible( const tile_type& type )
{
    bool traversible{ false };

    switch( type )
    {
    case tile_type::empty : traversible = true; break;
    case tile_type::wall : traversible = false; break;
    case tile_type::player_base : traversible = false; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return traversible;
}

bool tile_has_health( const tile_type& type )
{
    bool has_health{ false };

    switch( type )
    {
    case tile_type::empty : has_health = false; break;
    case tile_type::wall : has_health = false; break;
    case tile_type::player_base : has_health = true; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return has_health;
}

QString tile_image_path( const tile_type& type )
{
    QString image_name;

    switch( type )
    {
    case tile_type::empty : image_name = TILE_IMAGE_PATH_EMPTY; break;
    case tile_type::wall : image_name = TILE_IMAGE_PATH_WALL; break;
    case tile_type::player_base : image_name = TILE_IMAGE_PATH_PLAYER_BASE; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return QString( "qrc:/graphics/%1.png" ).arg( image_name );
}

constexpr QRect tile_rect( const QPoint& pos, const QSize& size ) noexcept
{
    return QRect{ pos, QPoint{ pos.x() + size.width(), pos.y() + size.height() } };
}

ecs::entity& create_entity_tile( const tile_type& type,
                                 const QPoint& pos,
                                 const QSize& size,
                                 ecs::world& world,
                                 uint32_t max_health )
{
    ecs::entity& entity = world.create_entity();

    try
    {
        entity.add_component< component::geometry >( tile_rect( pos, size ), 0 );
        entity.add_component< component::graphics >( tile_image_path( type ), true );

        if( !tile_traversible( type ) )
        {
            entity.add_component< component::non_traversible >();
        }

        if( tile_has_health( type ) )
        {
            if( !max_health )
            {
                throw std::invalid_argument{ "Tile has health but max health is set to 0" };
            }

            entity.add_component< component::health >( max_health );
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
