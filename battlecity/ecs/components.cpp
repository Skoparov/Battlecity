#include "components.h"

namespace game
{

namespace component
{

geometry::geometry( const QRect& rect, int rotation ) noexcept:
    m_rect( rect ), m_rotation( rotation ) {}

bool geometry::intersects_with( const geometry& other ) const noexcept
{
    return m_rect.intersects( other.m_rect );
}

void geometry::set_pos(const QPoint& point ) noexcept
{
    m_rect.setTopLeft( point );
}

QPoint geometry::get_pos() const noexcept
{
    return m_rect.topLeft();
}

void geometry::set_rotation( int rotation ) noexcept
{
    m_rotation = rotation;
}

int geometry::get_rotation() const noexcept
{
    return m_rotation;
}

const QRect &geometry::get_rect() const noexcept
{
    return m_rect;
}

//

health::health( uint32_t max_health ) noexcept:
    m_health( max_health ), m_max_health( max_health ){}

void health::increase( uint32_t value ) noexcept
{
    m_health = std::min( m_health + value, m_max_health );
}

void health::decrease( uint32_t value ) noexcept
{
    m_health = m_health >= value? m_health - value : 0;
}

uint32_t health::get_health() const noexcept
{
    return m_health;
}

uint32_t health::get_max_health() const noexcept
{
    return m_max_health;
}

bool health::alive() const noexcept
{
    return m_health != 0;
}

}// components

}// ecs
