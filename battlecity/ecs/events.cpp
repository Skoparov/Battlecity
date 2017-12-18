#include "events.h"

namespace game
{

namespace event
{

namespace _detail
{

void event_cause::set_cause_entity( ecs::entity& entity )
{
    m_cause_entity = &entity;
}

ecs::entity* event_cause::get_cause_entity() const noexcept
{
    return m_cause_entity;
}

void event_causes::add_cause_entity( ecs::entity& entity )
{
    m_cause_entities[ entity.get_id() ] = &entity;
}

bool event_causes::entity_present( ecs::entity_id id ) const
{
    return ( m_cause_entities.count( id )!= 0 );
}

const std::unordered_map< ecs::entity_id, ecs::entity* >& event_causes::get_cause_entities() const noexcept
{
    return m_cause_entities;
}

}// _detail

geometry_changed::geometry_changed( bool x_changed,
                                    bool y_changed,
                                    bool rotation_changed ) noexcept:
    m_x_changed( x_changed ),
    m_y_changed( y_changed ),
    m_rotation_changed( rotation_changed ){}

bool geometry_changed::x_is_changed() const noexcept
{
    return m_x_changed;
}

bool geometry_changed::y_is_changed() const noexcept
{
    return m_y_changed;
}

bool geometry_changed::rotation_is_changed() const noexcept
{
    return m_rotation_changed;
}

//

graphics_changed::graphics_changed( bool image_changed, bool visibility_changed ) noexcept:
    m_image_changed( image_changed ),
    m_visibility_changed( visibility_changed ){}

bool graphics_changed::image_changed() const noexcept
{
    return m_image_changed;
}

bool graphics_changed::visibility_changed() const noexcept
{
    return m_visibility_changed;
}

//

namespace _detail
{

kill::kill( ecs::entity& victim, const object_type& killer_type, ecs::entity* killer) noexcept:
    m_killer( killer ),
    m_victim( victim ),
    m_killer_type( killer_type ){}

ecs::entity* kill::get_killer() const noexcept
{
    return m_killer;
}

const object_type& kill::get_killer_type() const noexcept
{
    return m_killer_type;
}

ecs::entity& kill::get_victim() const noexcept
{
    return m_victim;
}

}// _detail

level_completed::level_completed( const level_game_result& result ) noexcept :
    m_level_result( result ){}

const level_game_result& level_completed::get_result() const noexcept
{
    return m_level_result;
}

//

projectile_fired::projectile_fired( ecs::entity& shooter, ecs::entity& projectile ) noexcept :
    m_shooter( shooter ),
    m_projectile( projectile ){}

ecs::entity& projectile_fired::get_shooter() const noexcept
{
    return m_shooter;
}

ecs::entity& projectile_fired::get_projectile() const noexcept
{
    return m_projectile;
}

// detail

}// events

}// ecs
