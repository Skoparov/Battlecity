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
    bool empty() const noexcept;

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
class action_done
{
public:
    action_done() = default;
    action_done( const object_type& subject_type,
                 ecs::entity& subject,
                 const object_type& performer_type,
                 ecs::entity* performer = nullptr ) noexcept;

    ecs::entity* get_performer() const noexcept;
    ecs::entity& get_subject() const noexcept;
    const object_type& get_performer_type() const noexcept;
    const object_type& get_subject_type() const noexcept;

private:
    object_type m_subject_type;
    ecs::entity* m_subject{ nullptr };
    object_type m_performer_type;
    ecs::entity* m_performer{ nullptr };
};

}// details

//

class entity_hit final : public _detail::action_done
{
public:
    using _detail::action_done::action_done;
};

class entity_killed final : public _detail::action_done
{
public:
    using _detail::action_done::action_done;
};

//

class powerup_taken final : public _detail::action_done
{
public:
    using _detail::action_done::action_done;
};


//

class projectile_collision final : public _detail::action_done
{
public:
    using _detail::action_done::action_done;
};

class animation_started final : public _detail::event_cause
{
public:
    animation_started( const animation_type& type ) : m_animation_type( type ){}

private:
    animation_type m_animation_type;
};

class animation_ended final : public _detail::event_cause
{
public:
    animation_ended( const animation_type& type ) : m_animation_type( type ){}

private:
    animation_type m_animation_type;
};

//

class entities_removed final
{
    using removed_entities_umap = std::unordered_map< object_type, std::list< ecs::entity* > >;

public:
    void add_entity( const object_type& type, ecs::entity& entity );
    const removed_entities_umap& get_removed_entities() const noexcept;
    bool empty() const noexcept;

private:
    removed_entities_umap m_removed_entities;
};

//

class entity_respawned final :  public _detail::event_cause{};

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
