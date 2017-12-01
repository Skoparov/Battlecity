#ifndef EVENTS_H
#define EVENTS_H

#include <list>

#include "framework/entity.h"

namespace game
{

namespace events
{

namespace detail
{

class causes
{
public:
    void add_entity( ecs::entity_id id );
    const std::list< ecs::entity_id >& get_entities() const noexcept;

private:
    std::list< ecs::entity_id > m_entities;
};

}// detail

class geometry_changed : public detail::causes
{
};

class entities_killed : public detail::causes{};

}// components

}// game

#endif
