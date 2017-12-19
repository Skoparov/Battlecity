#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <chrono>

#include <QRect>
#include <QString>

#include "ecs/framework/entity.h"
#include "general_enums.h"

namespace game
{

namespace component
{

class tile_object final
{
public:
    tile_object() = default;
    explicit tile_object( const tile_type& type ) noexcept;

    void set_tile_type( const tile_type& type ) noexcept;
    const tile_type& get_tile_type() const noexcept;

private:
    tile_type m_tile_type{ tile_type::empty };
};

//

class turret_object final
{
    using clock = std::chrono::high_resolution_clock;

public:
    turret_object( const std::chrono::milliseconds& cooldown ) noexcept;

    bool set_fire_status( bool fired ) noexcept;
    bool has_fired() const noexcept;

private:
    bool m_fired{ false };
    clock::time_point m_last_fired{};
    std::chrono::milliseconds m_cooldown{ 0 };
};

//

class tank_object final{};

//

class game_map final{};

//

class player final{};

//

class enemy final{};

//

class player_base final{};

//

class explosion final{};

//

class frag final
{
public:
    explicit frag( uint32_t num ) noexcept;
    uint32_t get_num() const noexcept;

private:
    uint32_t m_num{ 0 };
};

//

class projectile final
{
public:
    projectile() = default;
    projectile( uint32_t damage, const ecs::entity& owner );

    void set_damage( uint32_t damage ) noexcept;
    uint32_t get_damage() const noexcept;

    ecs::entity_id get_shooter_id() const noexcept;
    const object_type& get_shooter_type() const noexcept;

private:
    uint32_t m_damage{ 1 };
    ecs::entity_id m_shooter_id{ INVALID_NUMERIC_ID };
    object_type m_shooter_type;
};

//

class geometry final
{
public:
    geometry() = default;
    geometry( const QRect& rect, int rotation = 0 ) noexcept;

    bool intersects_with( const geometry& other ) const noexcept;
    bool intersects_with( const QRect& rect ) const noexcept;

    void set_pos( const QPoint& point ) noexcept;
    QPoint get_pos() const noexcept;

    void set_size( const QSize& size ) noexcept;
    QSize get_size() const noexcept;

    void set_rotation( int rotation ) noexcept;
    int get_rotation() const noexcept;

    void set_rect( const QRect& rect ) noexcept;
    const QRect& get_rect() const noexcept;

private:
    QRect m_rect;
    int m_rotation{ 0 };
};

//

class movement final
{
public:
    movement() = default;
    movement( uint32_t step, const movement_direction& direction = movement_direction::none ) noexcept;

    void set_speed( uint32_t speed ) noexcept;
    uint32_t get_speed() const noexcept;

    void set_move_direction( const movement_direction& direction ) noexcept;
    const movement_direction& get_move_direction() const noexcept;
private:
    uint32_t m_speed{ 0 };
    movement_direction m_move_direction{ movement_direction::none };
};

//

class non_traversible final{};
class flying final{}; // can pass through non_traversible

//

class graphics final
{
public:
    graphics() = default;
    graphics( const QString& image_path, bool visible = true );

    void set_image_path( const QString& image_path );
    const QString& get_image_path() const noexcept;

    void set_visible( bool visible ) noexcept;
    bool get_visible() const noexcept;

private:
    QString m_image_path;
    bool m_visible{ true };
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

class lifes final
{
public:
    lifes() = default;
    explicit lifes( const has_infinite_lifes& mode, uint32_t lifes = 0 ) noexcept;

    void increase( uint32_t value ) noexcept;
    void decrease( uint32_t value ) noexcept;

    bool has_life() const noexcept;
    uint32_t get_lifes() const noexcept;
    const has_infinite_lifes& get_mode() const noexcept;

private:
    has_infinite_lifes m_mode{ has_infinite_lifes::no };
    uint32_t m_lifes{ 0 };
};

//

class kills_counter final
{
public:
    void increase( uint32_t value ) noexcept;
    uint32_t get_kills() const noexcept;

private:
    uint32_t m_kills{ 0 };
};

class respawn_point{};

}// components

}// game

#endif
