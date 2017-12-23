#include "graphics_map_object.h"

#include <mutex>

#include  "ecs/components.h"
#include "ecs/framework/details/rw_lock_guard.h"

namespace game
{

graphics_map_object::graphics_map_object( ecs::entity* entity, const object_type& type, QObject* parent ):
    base_map_object( entity, type, parent )
{
    ecs::world& world = m_entity->get_world();
    world.subscribe< event::graphics_changed >( *this );
}

graphics_map_object::~graphics_map_object()
{
    ecs::world& world = m_entity->get_world();
    world.unsubscribe< event::graphics_changed >( *this );
}

const QString& graphics_map_object::get_image_path() const noexcept
{
    component::graphics& g = m_entity->get_component_unsafe< component::graphics >();
    ecs::rw_lock_guard< ecs::rw_lock> l{ g, ecs::lock_mode::read };
    return g.get_image_path();
}

bool graphics_map_object::get_visible() const noexcept
{
    component::graphics& g = m_entity->get_component_unsafe< component::graphics >();
    ecs::rw_lock_guard< ecs::rw_lock> l{ g, ecs::lock_mode::read };
    return g.get_visible();
}

void graphics_map_object::on_event( const event::graphics_changed& event )
{
    if( event.get_cause_entity() == m_entity )
    {
        component::graphics& g = m_entity->get_component_unsafe< component::graphics >();
        ecs::rw_lock_guard< ecs::rw_lock> l{ g, ecs::lock_mode::read };

        if( event.visibility_changed() )
        {
            emit visibility_changed( g.get_visible() );
        }

        if( event.image_changed() )
        {
            emit image_changed( g.get_image_path() );
        }
    }
}

}// game
