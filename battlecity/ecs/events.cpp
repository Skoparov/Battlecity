#include "events.h"

namespace game
{

namespace event
{

namespace detail
{

void causes::add_entity( ecs::entity_id id )
{
    m_entities.emplace( id );
}

bool causes::entity_present( ecs::entity_id id ) const
{
    return ( m_entities.count( id )!= 0 );
}

const std::unordered_set< ecs::entity_id >& causes::get_entities() const noexcept
{
    return m_entities;
}

}// detail

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
