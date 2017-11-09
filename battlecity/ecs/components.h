#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QRect>
#include <QString>

namespace game
{

namespace component
{

class projectile{};

class geometry final
{
public:
    geometry() = default;
    geometry( const QRect& rect, int rotation ) noexcept;

    bool intersects_with( const geometry& other ) const noexcept;

    void set_pos( const QPoint& point ) noexcept;
    QPoint get_pos() const noexcept;

    void set_rotation( int rotation ) noexcept;
    int get_rotation() const noexcept;

    const QRect& get_rect() const noexcept;

private:
    QRect m_rect;
    int m_rotation{ 0 };
};

class health final
{
public:
    health() = default;
    health( uint32_t max_health ) noexcept;

    void increase( uint32_t value ) noexcept;
    void decrease( uint32_t value ) noexcept;

    uint32_t get_health() const noexcept;
    uint32_t get_max_health() const noexcept;

    bool alive() const noexcept;

private:
    uint32_t m_health{ 0 };
    const uint32_t m_max_health{ 0 };
};

struct object_graphics
{
    QString image_path;
};

}// components

}// game

#endif
