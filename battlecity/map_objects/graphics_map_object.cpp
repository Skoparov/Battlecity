#include "graphics_map_object.h"

graphics_map_object::graphics_map_object( const QString& image_path,
                                          const bool& visible,
                                          const bool& traversible,
                                          QObject* parent ):
    base_map_object( traversible, parent ),
    m_image_path( image_path ),
    m_visible( visible ){}

void graphics_map_object::set_image_path( const QString& path )
{
    m_image_path = path;
}

const QString& graphics_map_object::get_image_path() const noexcept
{
    return m_image_path;
}

void graphics_map_object::set_visible( bool visible ) noexcept
{
    m_visible = visible;
    emit visible_changed( visible );
}

bool graphics_map_object::get_visible() const noexcept
{
    return m_visible;
}



