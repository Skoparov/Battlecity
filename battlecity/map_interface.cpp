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
    m_tiles = m_controller.get_tiles();
    return QQmlListProperty< tile_map_object >{ this, m_tiles };
}

QQmlListProperty< graphics_map_object > qml_map_interface::get_player_bases()
{
    m_player_bases = m_controller.get_player_bases();
    return QQmlListProperty< graphics_map_object >{ this, m_player_bases };
}

QQmlListProperty< tank_map_object > qml_map_interface::get_player_tanks()
{
    m_player_tanks = m_controller.get_player_tanks();
    return QQmlListProperty< tank_map_object >{ this, m_player_tanks };
}

}// game
