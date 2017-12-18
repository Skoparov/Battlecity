#ifndef EVENTS_H
#define EVENTS_H

#include <unordered_set>

#include "framework/entity.h"
#include "general_enums.h"

namespace game
{

enum class level_game_result{ victory, defeat };

namespace event
{

namespace _detail
{

class event_cause_id
{
public:
    void set_cause_id( ecs::entity_id id ) noexcept;
    ecs::entity_id get_cause_id() const noexcept;

private:
    ecs::entity_id m_cause_id{ INVALID_NUMERIC_ID };
};

 // contains entites that caused the event
class event_cause
{
public:
    void set_cause_entity( ecs::entity& entity ) noexcept;
    ecs::entity* get_cause_entity() const noexcept;

private:
    ecs::entity* m_cause_entity{ nullptr };
};

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

//

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

class kill
{
public:
    kill( ecs::entity& victim, const object_type& killer_type, ecs::entity* killer = nullptr ) noexcept;

    ecs::entity* get_killer() const noexcept;
    const object_type& get_killer_type() const noexcept;
    ecs::entity& get_victim() const noexcept;

private:
    ecs::entity* m_killer{ nullptr };
    ecs::entity& m_victim;
    object_type m_killer_type;
};

}// details

class player_killed final : public _detail::kill
{
public:
    using _detail::kill::kill;
};

//

class enemy_killed final : public _detail::kill
{
public:
    using _detail::kill::kill;
};

//

class player_base_killed final{};

//

class projectile_collision final : public _detail::event_cause{};
class explosion_started final : public _detail::event_cause{};
class explosion_ended final : public _detail::event_cause_id{};
//

class entities_removed final : public _detail::event_causes{};

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
