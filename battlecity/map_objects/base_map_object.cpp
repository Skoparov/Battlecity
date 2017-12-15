#include "base_map_object.h"

#include "ecs/components.h"

namespace game
{

base_map_object::base_map_object( ecs::entity* entity, const object_type& type, QObject* parent ):
    QObject( parent ),
    m_entity( entity ),
    m_object_type( type )
{
    if( !m_entity )
    {
        throw std::invalid_argument{ "Map object entity is null" };
    }
}

ecs::entity_id base_map_object::get_id() const noexcept
{
    return m_entity->get_id();
}

const object_type& base_map_object::get_type() const noexcept
{
    return m_object_type;
}

int base_map_object::get_position_x() const noexcept
{
    const component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    return g.get_pos().x();
}

int base_map_object::get_position_y() const noexcept
{
    const component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    return g.get_pos().y();
}

int base_map_object::get_width() const noexcept
{
    const component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    return g.get_size().width();
}

int base_map_object::get_height() const noexcept
{
    const component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    return g.get_size().height();
}

int base_map_object::get_rotation() const noexcept
{
    const component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    return g.get_rotation();
}

bool base_map_object::get_traversible() const noexcept
{
    return !m_entity->has_component< component::non_traversible >();
}

}// game
