#include "tank_map_object.h"

namespace game
{

tank_map_object::tank_map_object(ecs::entity* entity, const object_type& type, QObject* parent ):
    movable_map_object( entity, type, parent )
{
    if( type != object_type::player_tank && type != object_type::enemy_tank )
    {
        throw std::invalid_argument{ "Invalid tank type" };
    }
}

const tank_type& tank_map_object::get_tank_type() const noexcept
{
    const component::tank_object& t = m_entity->get_component_unsafe< component::tank_object >();
    return t.get_tank_type();
}

}// game
