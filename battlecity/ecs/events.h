#ifndef EVENTS_H
#define EVENTS_H

#include <unordered_set>

#include "framework/entity.h"

namespace game
{

namespace event
{

namespace detail
{

 // contains entites that caused the event
class causes
{
public:
    void add_entity( ecs::entity_id id );
    bool entity_present( ecs::entity_id id ) const;

private:
    std::unordered_set< ecs::entity_id > m_entities;
};

}// detail

class geometry_changed final : public detail::causes
{
public:
    geometry_changed( bool x_changed, bool y_changed, bool rotation_changed ) noexcept;

    bool x_is_changed() const noexcept;
    bool y_is_changed() const noexcept;
    bool rotation_is_changed() const noexcept;

private:
    bool m_x_changed{ false };
    bool m_y_changed{ false };
    bool m_rotation_changed{ false };
};

}// events

}// game

#endif
