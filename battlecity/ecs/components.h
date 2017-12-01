#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QRect>
#include <QString>

namespace game
{

enum class tile_type{ wall, empty, player_base };
enum class tank_type{ player, enemy };

namespace component
{

class tile_object
{
public:
    tile_object() = default;
    tile_object( const tile_type& type ) noexcept;

    void set_tile_type( const tile_type& type ) noexcept;
    const tile_type& get_tile_type() const noexcept;

private:
    tile_type m_tile_type{ tile_type::empty };
};

//

class tank_object
{
public:
    tank_object() = default;
    tank_object( const tank_type& type ) noexcept;

    void set_tank_type( const tank_type& type ) noexcept;
    const tank_type& get_tank_type() const noexcept;

private:
    tank_type m_tank_type{ tank_type::enemy };
};

//

class geometry final
{
public:
    geometry() = default;
    geometry( const QRect& rect, int rotation ) noexcept;

    bool intersects_with( const geometry& other ) const noexcept;

    void set_pos( const QPoint& point ) noexcept;
    const QPoint& get_pos() const noexcept;

    void set_size( const QSize& size ) noexcept;
    const QSize& get_size() const noexcept;

    void set_rotation( int rotation ) noexcept;
    int get_rotation() const noexcept;

    const QRect& get_rect() const noexcept;

private:
    QRect m_rect;
    int m_rotation{ 0 };
};

//

class non_traversible final{};

//

class graphics final
{
public:
    graphics() = default;
    graphics( const QString& image_path, bool visible );

    void set_image_path( const QString& image_path );
    const QString& get_image_path() const noexcept;

    void set_visible( bool visible ) noexcept;
    bool get_visible() const noexcept;

private:
    bool m_visible{ true };
    QString m_image_path;
};

//

class health final
{
public:
    health() = default;
    explicit health( uint32_t max_health ) noexcept;

    void increase( uint32_t value ) noexcept;
    void decrease( uint32_t value ) noexcept;

    uint32_t get_health() const noexcept;
    uint32_t get_max_health() const noexcept;

    bool alive() const noexcept;

private:
    uint32_t m_health{ 0 };
    const uint32_t m_max_health{ 0 };
};

}// components

}// game

#endif
