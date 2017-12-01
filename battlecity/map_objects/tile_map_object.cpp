#include "tile_map_object.h"

#define TILE_IMAGE_PATH_EMPTY "tile_empty"
#define TILE_IMAGE_PATH_WALL "tile_wall"
#define TILE_IMAGE_PATH_PLAYER_BASE "tile_player_base"

bool tile_type_to_visible( const tile_type& type )
{
    bool visible{ false };

    switch( type )
    {
    case tile_type::empty : visible = true; break;
    case tile_type::wall : visible = true; break;
    case tile_type::player_base : visible = true; break;
    default: throw std::invalid_argument{ "Unimplemented tile type" };
    }

    return visible;
}

bool tile_type_to_traversible( const tile_type& type )
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

QString tile_type_to_image_path( const tile_type& type )
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

tile_map_object::tile_map_object( const tile_type& type, QObject* parent ):
    graphics_map_object( tile_type_to_image_path( type ),
                         tile_type_to_visible( type ),
                         tile_type_to_traversible( type ),
                         parent ),
    m_type( type ){}

const tile_type &tile_map_object::get_tile_type() const noexcept
{
    return m_type;
}
