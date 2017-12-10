#include "tile_map_object.h"

namespace game
{

tile_map_object::tile_map_object( ecs::entity* entity, QObject* parent ):
    graphics_map_object( entity, object_type::tile, parent ){}

const tile_type& tile_map_object::get_tile_type() const noexcept
{
    const component::tile_object& t = m_entity->get_component_unsafe< component::tile_object >();
    return t.get_tile_type();
}

}// game
