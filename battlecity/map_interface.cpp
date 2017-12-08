#include "map_interface.h"

#include "controller.h"

namespace game
{

qml_map_interface::qml_map_interface( controller&controller,
                                      QObject* parent ) :
    QObject( parent ),
    m_controller( controller ){}

int qml_map_interface::get_rows_count() const noexcept
{
    return m_controller.get_rows_count();
}

int qml_map_interface::get_columns_count() const noexcept
{
    return m_controller.get_columns_count();
}

int qml_map_interface::get_tile_width() const noexcept
{
    return m_controller.get_tile_width();
}

int qml_map_interface::get_tile_height() const noexcept
{
    return m_controller.get_tile_height();
}

QQmlListProperty< tile_map_object > qml_map_interface::get_tiles()
{
    static QList< tile_map_object* > list = m_controller.get_tiles();
    return QQmlListProperty< tile_map_object >{ this, list };
}

}// game
