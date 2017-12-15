#include "tank_map_object.h"

#include "ecs/components.h"

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

bool tank_map_object::set_fired( bool fired ) noexcept
{
    component::turret& t = m_entity->get_component_unsafe< component::turret >();
    return t.set_fire_status( fired );
}

bool tank_map_object::get_fired() const noexcept
{
    const component::turret& t = m_entity->get_component_unsafe< component::turret >();
    return t.has_fired();
}

}// game
