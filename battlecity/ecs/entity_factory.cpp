#include "entity_factory.h"

#define TILE_IMAGE_PATH_EMPTY "tile_empty"
#define TILE_IMAGE_PATH_WALL "tile_wall"

namespace game
{

ecs::entity& create_world_entity( const QSize& tile_size, const QSize& map_size, ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    entity.add_component< component::map_object >();

    QRect map_rect{ 0, 0,
                    tile_size.width() * map_size.width(),
                    tile_size.height() * map_size.height() };

    entity.add_component< component::geometry >( map_rect );

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
    case tile_type::empty : image_name = TILE_IMAGE_PATH_EMPTY; break;
    case tile_type::wall : image_name = TILE_IMAGE_PATH_WALL; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return QString( "qrc:/graphics/%1.png" ).arg( image_name );
}

constexpr QRect tile_rect( const QPoint& pos, const QSize& size ) noexcept
{
    return QRect{ pos, QPoint{ pos.x() + size.width(), pos.y() + size.height() } };
}

ecs::entity& create_entity_tile(const tile_type& type,
                                 const QPoint& pos,
                                 const QSize& size,
                                 ecs::world& world )
{
    ecs::entity& entity = world.create_entity();

    try
    {
        entity.add_component< component::geometry >( tile_rect( pos, size ) );
        entity.add_component< component::graphics >( tile_image_path( type ), true );

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
