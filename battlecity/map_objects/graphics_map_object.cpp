#include "graphics_map_object.h"

#include  "ecs/components.h"

namespace game
{

graphics_map_object::graphics_map_object( ecs::entity& entity, const object_type& type, QObject* parent ):
    base_map_object( entity, type, parent ){}

void graphics_map_object::set_image_path( const QString& /*path*/ )
{
    assert( false );
}

const QString& graphics_map_object::get_image_path() const noexcept
{
    const component::graphics& g = m_entity.get_component< component::graphics >();
    return g.get_image_path();
}

void graphics_map_object::set_visible( bool /*visible*/ ) noexcept
{
    assert( false );
}

bool graphics_map_object::get_visible() const noexcept
{
    const component::graphics& g = m_entity.get_component< component::graphics >();
    return g.get_visible();
}

}// game
