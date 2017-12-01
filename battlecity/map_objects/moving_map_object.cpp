#include "base_map_object.h"

base_map_object::base_map_object( bool traversible, QObject* parent ):
    QObject( parent ),
    m_traversible( traversible ){}

void base_map_object::set_position_x( int x ) noexcept
{
    m_position.setX( x );
    emit pos_x_changed( x );
}

int base_map_object::get_position_x() const noexcept
{
    return m_position.x();
}

void base_map_object::set_position_y( int y ) noexcept
{
    m_position.setY( y );
    emit pos_y_changed( y );
}

int base_map_object::get_position_y() const noexcept
{
    return m_position.y();
}

int base_map_object::get_width() const noexcept
{
    return m_width;
}

int base_map_object::get_height() const noexcept
{
    return m_height;
}

void base_map_object::set_rotation( int rotation ) noexcept
{
    m_rotation = rotation;
    emit rotation_changed( rotation );
}

int base_map_object::get_rotation() const noexcept
{
    return m_rotation;
}

bool base_map_object::get_traversible() const noexcept
{
    return m_traversible;
}
