#include "events.h"

namespace game
{

namespace events
{

namespace detail
{

void causes::add_entity( ecs::entity_id id )
{
    m_entities.emplace_back( id );
}

const std::list< ecs::entity_id >& causes::get_entities() const noexcept
{
    return m_entities;
}

}// detail

}// components

}// ecs
