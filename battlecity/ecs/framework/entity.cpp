#include "entity.h"

namespace ecs
{

entity::entity( world* world, entity_id id ) noexcept:
    m_world( world ),
    m_id( id ){}

entity_id entity::get_id() const noexcept
{
    return m_id;
}

bool operator==( const entity& l, const entity& r ) noexcept
{
    return l.get_id() == r.get_id();
}

bool operator!=( const entity& l, const entity& r ) noexcept
{
    return !( l == r );
}

}// ecs
