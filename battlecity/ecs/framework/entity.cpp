#include "entity.h"

namespace ecs
{

entity::entity( entity_id id ) noexcept: m_id( id ){}

bool entity::has_component( const component_id& id ) const
{
    return m_components.count( id ) != 0;
}

entity_id entity::id() const noexcept
{
    return m_id;
}

}// ecs
