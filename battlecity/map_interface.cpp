#include "map_interface.h"

namespace game
{

qml_map_interface::qml_map_interface( ecs::world& world, QObject* parent ) :
    QObject( parent ),
    m_world( world ){}

void qml_map_interface::load_level(uint32_t level)
{
    m_map_data = read_map_file( QString{ ":/maps/map_%1" }.arg( level ), m_world );
}

int qml_map_interface::get_rows_count() const noexcept
{
    return m_map_data.get_rows_count();
}

int qml_map_interface::get_columns_count() const noexcept
{
    return m_map_data.get_columns_count();
}

int qml_map_interface::get_tile_width() const noexcept
{
    return m_map_data.get_tile_width();
}

int qml_map_interface::get_tile_height() const noexcept
{
    return m_map_data.get_tile_height();
}

QQmlListProperty< tile_map_object > qml_map_interface::get_tiles()
{
    QList< tile_map_object* > list{ };

     m_map_data.get_objects_of_type< object_type::tile >();
    return QQmlListProperty< tile_map_object >{ this, list };
}

void qml_map_interface::clear()
{
    m_map_data = {};
}

}// game
