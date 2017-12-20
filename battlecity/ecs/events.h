#ifndef EVENTS_H
#define EVENTS_H

#include <list>
#include <unordered_set>

#include "framework/entity.h"
#include "general_enums.h"

namespace std
{

template<> struct hash< game::object_type >
{
    using argument_type = game::object_type;
    using underlying_type = std::underlying_type< argument_type >::type;
    using result_type = std::hash< underlying_type >::result_type;

    size_t operator()( const argument_type& arg ) const
    {
        std::hash< underlying_type > hasher;
        return hasher( static_cast< underlying_type >( arg ) );
    }
};

}// std

namespace game
{

enum class level_game_result{ victory, defeat };

namespace event
{

namespace _detail
{

// contains the id of entity that caused the event
class event_cause_id
{
public:
    void set_cause_id( ecs::entity_id id ) noexcept;
    ecs::entity_id get_cause_id() const noexcept;

private:
    ecs::entity_id m_cause_id{ INVALID_NUMERIC_ID };
};

 // contains the entity that caused the event
class event_cause
{
public:
    void set_cause_entity( ecs::entity& entity ) noexcept;
    ecs::entity* get_cause_entity() const noexcept;

private:
    ecs::entity* m_cause_entity{ nullptr };
};

// contains the entities that caused the event
class event_causes
{
public:
    void add_cause_entity( ecs::entity& entity );
    bool entity_present( ecs::entity_id id ) const;
    const std::unordered_map< ecs::entity_id, ecs::entity* >& get_cause_entities() const noexcept;

private:
    std::unordered_map< ecs::entity_id, ecs::entity* > m_cause_entities;
};

}// detail

class geometry_changed final : public _detail::event_cause
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

//

class graphics_changed final : public _detail::event_cause
{
public:
    graphics_changed( bool image_changed, bool visibility_changed ) noexcept;

    bool image_changed() const noexcept;
    bool visibility_changed() const noexcept;

private:
    bool m_image_changed{ false };
    bool m_visibility_changed{ false };
};

//

namespace _detail
{

// Contains information of the collision
class projectile_hit_info
{
public:
    projectile_hit_info() = default;
    projectile_hit_info( const object_type& victim_type,
                         ecs::entity& victim,
                         const object_type& shooter_type,
                         ecs::entity* shooter = nullptr ) noexcept;

    ecs::entity* get_shooter() const noexcept;
    ecs::entity& get_victim() const noexcept;
    const object_type& get_shooter_type() const noexcept;
    const object_type& get_victim_type() const noexcept;

private:
    object_type m_victim_type;
    ecs::entity* m_victim{ nullptr };
    object_type m_shooter_type;
    ecs::entity* m_shooter{ nullptr };
};

}// details

//

class entity_hit final : public _detail::projectile_hit_info
{
public:
    using _detail::projectile_hit_info::projectile_hit_info;
};

class entity_killed final : public _detail::projectile_hit_info
{
public:
    using _detail::projectile_hit_info::projectile_hit_info;
};

//

class projectile_collision final : public _detail::event_cause{};
class explosion_started final : public _detail::event_cause{};
class explosion_ended final : public _detail::event_cause_id{};

//

class entities_removed final
{
    using removed_entities_umap = std::unordered_map< object_type, std::list< ecs::entity* > >;

public:
    void add_entity( const object_type& type, ecs::entity& entity );
    const removed_entities_umap& get_removed_entities() const noexcept;

private:
    removed_entities_umap m_removed_entities;
};

//

class level_completed final
{
public:
    explicit level_completed( const level_game_result& result ) noexcept;
    const level_game_result& get_result() const noexcept;

private:
    level_game_result m_level_result;
};

//

class projectile_fired final
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
