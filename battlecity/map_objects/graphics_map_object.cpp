#include "graphics_map_object.h"

#include  "ecs/components.h"

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

void graphics_map_object::set_image_path( const QString& /*path*/ )
{
    assert( false );
}

const QString& graphics_map_object::get_image_path() const noexcept
{
    const component::graphics& g = m_entity->get_component_unsafe< component::graphics >();
    return g.get_image_path();
}

void graphics_map_object::set_visible( bool /*visible*/ ) noexcept
{
    assert( false );
}

bool graphics_map_object::get_visible() const noexcept
{
    const component::graphics& g = m_entity->get_component_unsafe< component::graphics >();
    return g.get_visible();
}

void graphics_map_object::on_event( const event::graphics_changed& event )
{
    if( event.entity_present( m_entity->get_id() ) )
    {
        const component::graphics& g = m_entity->get_component_unsafe< component::graphics >();

        if( event.image_changed() )
        {
            emit image_changed( g.get_image_path() );
        }

        if( event.visibility_changed() )
        {
            emit visibility_changed( g.get_visible() );
        }
    }
}

}// game
