#include "events.h"

namespace game
{

namespace event
{

namespace _detail
{

void event_cause_id::set_cause_id( ecs::entity_id id ) noexcept
{
    m_cause_id = id;
}

ecs::entity_id event_cause_id::get_cause_id() const noexcept
{
    return m_cause_id;
}

void event_cause::set_cause_entity( ecs::entity& entity ) noexcept
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

projectile_hit_info::projectile_hit_info( const object_type& victim_type,
                                ecs::entity& victim,
                                const object_type& shooter_type,
                                ecs::entity* shooter ) noexcept:
    m_victim_type( victim_type ),
    m_victim( &victim ),
    m_shooter_type( shooter_type ),
    m_shooter( shooter ){}

ecs::entity* projectile_hit_info::get_shooter() const noexcept
{
    return m_shooter;
}

ecs::entity& projectile_hit_info::get_victim() const noexcept
{
    return *m_victim;
}

const object_type& projectile_hit_info::get_shooter_type() const noexcept
{
    return m_shooter_type;
}

const object_type& projectile_hit_info::get_victim_type() const noexcept
{
    return m_victim_type;
}

}// _detail

void entities_removed::add_entity( const object_type& type, ecs::entity& entity )
{
    m_removed_entities[ type ].emplace_back( &entity );
}

auto entities_removed::get_removed_entities() const noexcept -> const removed_entities_umap&
{
    return m_removed_entities;
}

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
