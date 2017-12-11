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
    const std::unordered_set< ecs::entity_id >& get_entities() const noexcept;

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

class entities_removed final : public detail::causes{};

class projectile_fired final : public detail::causes
{
public:
    explicit projectile_fired( ecs::entity& shooter, ecs::entity& projectile ) noexcept;
    ecs::entity& get_shooter() const noexcept;
    ecs::entity& get_projectile() const noexcept;

private:
    ecs::entity& m_shooter;
    ecs::entity& m_projectile;
};

}// events

}// game

#endif
