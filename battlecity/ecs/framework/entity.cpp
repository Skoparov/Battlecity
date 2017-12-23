#include "entity.h"

#include "world.h"

namespace ecs
{

entity::entity( world* world, entity_id id ) noexcept:
    m_world( world ),
    m_id( id ){}

void entity::add_component_to_world( const component_id& id, component_wrapper& w )
{
    m_world->add_component( *this, id, w );
}

void entity::remove_component_from_world( const component_id& id )
{
    m_world->remove_component( *this, id );
}

void entity::set_state( const entity_state& state ) noexcept
{
    m_state = state;
}

const entity_state& entity::get_state() const noexcept
{
    return m_state;
}

entity_id entity::get_id() const noexcept
{
    return m_id;
}

world& entity::get_world() noexcept
{
    return *m_world;
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
