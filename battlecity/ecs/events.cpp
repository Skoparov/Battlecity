#include "events.h"

namespace game
{

namespace events
{

namespace detail
{

void entity_caused_event::add_target_entity( ecs::entity_id id )
{
    m_target_entities.emplace_back( id );
}

const std::vector< ecs::entity_id > entity_caused_event::get_cause_ids() const noexcept
{
    return m_target_entities;
}

}// detail

}// components

}// ecs
