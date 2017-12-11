#include "tank_map_object.h"

namespace game
{

tank_map_object::tank_map_object( ecs::entity* entity, const object_type& type, QObject* parent ):
    movable_map_object( entity, type, parent )
{
    if( type != object_type::player_tank && type != object_type::enemy_tank )
    {
        throw std::invalid_argument{ "Invalid tank type" };
    }
}

void tank_map_object::set_fired( bool fired ) noexcept
{
    component::tank_object& t = m_entity->get_component_unsafe< component::tank_object >();
    return t.set_fire_status( fired );
}

bool tank_map_object::get_fired() const noexcept
{
    const component::tank_object& t = m_entity->get_component_unsafe< component::tank_object >();
    return t.has_fired();
}

const tank_type& tank_map_object::get_tank_type() const noexcept
{
    const component::tank_object& t = m_entity->get_component_unsafe< component::tank_object >();
    return t.get_tank_type();
}

}// game
