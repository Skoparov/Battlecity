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

// detail

}// events

}// ecs
