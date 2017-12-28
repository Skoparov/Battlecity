#include "base_map_object.h"

#include <mutex>

#include "ecs/components.h"
#include "ecs/framework/details/rw_lock_guard.h"

namespace game
{

base_map_object::base_map_object( ecs::entity* entity, const object_type& type, QObject* parent ):
    QObject( parent ),
    m_entity( entity ),
    m_object_type( type )
{
    if( !m_entity )
    {
        throw std::invalid_argument{ "Map object entity is null" };
    }

    ecs::world& world = m_entity->get_world();
    world.subscribe< event::geometry_changed >( *this );
}

base_map_object::~base_map_object()
{
    ecs::world& world = m_entity->get_world();
    world.unsubscribe< event::geometry_changed >( *this );
    m_entity->get_world().schedule_remove_entity( *m_entity );
}

ecs::entity_id base_map_object::get_id() const noexcept
{
    return m_entity->get_id();
}

unsigned int base_map_object::get_qml_adapted_id() const noexcept
{
    return m_entity->get_id();
}

const object_type& base_map_object::get_type() const noexcept
{
    return m_object_type;
}

int base_map_object::get_position_x() const noexcept
{
    component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    ecs::rw_lock_guard< ecs::rw_lock > l{ g, ecs::lock_mode::read };
    return g.get_pos().x();
}

int base_map_object::get_position_y() const noexcept
{
    component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    ecs::rw_lock_guard< ecs::rw_lock > l{ g, ecs::lock_mode::read };
    return g.get_pos().y();
}

int base_map_object::get_width() const noexcept
{
    component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    ecs::rw_lock_guard< ecs::rw_lock > l{ g, ecs::lock_mode::read };
    return g.get_size().width();
}

int base_map_object::get_height() const noexcept
{
    component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    ecs::rw_lock_guard< ecs::rw_lock > l{ g, ecs::lock_mode::read };
    return g.get_size().height();
}

int base_map_object::get_rotation() const noexcept
{
    component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
    ecs::rw_lock_guard< ecs::rw_lock > l{ g, ecs::lock_mode::read };
    return g.get_rotation();
}

bool base_map_object::get_traversible() const noexcept
{
    ecs::rw_lock_guard< ecs::rw_lock > l{ *m_entity, ecs::lock_mode::read };
    return !m_entity->has_component< component::non_traversible >();
}

void base_map_object::on_event( const event::geometry_changed& event )
{
    if( event.get_cause_entity() == m_entity )
    {
        QPoint pos;
        int rotation{ 0 };

        {
            component::geometry& g = m_entity->get_component_unsafe< component::geometry >();
            ecs::rw_lock_guard< ecs::rw_lock > l{ g, ecs::lock_mode::read };
            pos = g.get_pos();
            rotation = g.get_rotation();
        }

        if( event.x_is_changed() )
        {
            emit pos_x_changed( pos.x() );
        }

        if( event.y_is_changed() )
        {
            emit pos_y_changed( pos.y() );
        }

        if( event.rotation_is_changed() )
        {
            emit rotation_changed( rotation );
        }
    }
}

}// game
