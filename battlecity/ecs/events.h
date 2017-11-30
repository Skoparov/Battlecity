#ifndef EVENTS_H
#define EVENTS_H

#include <unordered_set>

#include "framework/entity.h"

namespace game
{

namespace events
{

namespace detail
{

class entity_caused_event
{
public:
    entity_caused_event() = default;

    void add_target_entity( ecs::entity_id id );
    const std::vector< ecs::entity_id >& get_entities() const noexcept;

private:
    std::vector< ecs::entity_id > m_entities;
};

}// detail

class entities_killed : public detail::entity_caused_event
{
public:
    using detail::entity_caused_event::entity_caused_event;
};

}// components

}// game

#endif
