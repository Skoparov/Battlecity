#include "components.h"

namespace game
{

namespace component
{

tile_object::tile_object( const tile_type& type ) noexcept :
    m_tile_type( type ){}

void tile_object::set_tile_type( const tile_type& type ) noexcept
{
    m_tile_type = type;
}

const tile_type& tile_object::get_tile_type() const noexcept
{
    return m_tile_type;
}

//

void turret_object::set_fire_status( bool fired ) noexcept
{
    m_fired = fired;
}

bool turret_object::has_fired() const noexcept
{
    return m_fired;
}

//

tank_object::tank_object( const tank_type& type ) noexcept :
    m_tank_type( type ){}

void tank_object::set_tank_type( const tank_type& type ) noexcept
{
    m_tank_type = type;
}

const tank_type& tank_object::get_tank_type() const noexcept
{
    return m_tank_type;
}

//

projectile::projectile( uint32_t damage, ecs::entity_id owner ) noexcept :
    m_damage( damage ),
    m_owner( owner ){}

void projectile::set_damage( uint32_t damage ) noexcept
{
    m_damage = damage;
}

uint32_t projectile::get_damage() const noexcept
{
    return m_damage;
}

ecs::entity_id projectile::get_owner() const noexcept
{
    return m_owner;
}

//

geometry::geometry( const QRect& rect, int rotation ) noexcept:
    m_rect( rect ),
    m_rotation( rotation ){}

bool geometry::intersects_with( const geometry& other ) const noexcept
{
    return intersects_with( other.m_rect );
}

bool geometry::intersects_with( const QRect& rect ) const noexcept
{
    return m_rect.intersects( rect );
}

void geometry::set_pos(const QPoint& point ) noexcept
{
    m_rect.moveTo( point );
}

QPoint geometry::get_pos() const noexcept
{
    return m_rect.topLeft();
}

void geometry::set_size( const QSize& size ) noexcept
{
    m_rect.setSize( size );
}

QSize geometry::get_size() const noexcept
{
    return m_rect.size();
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

movement::movement( uint32_t speed,
                    const movement_direction& direction ) noexcept:
    m_speed( speed ),
    m_move_direction( direction ){}

void movement::set_speed( uint32_t speed ) noexcept
{
    m_speed = speed;
}

uint32_t movement::get_speed() const noexcept
{
    return m_speed;
}

void movement::set_move_direction( const movement_direction& direction ) noexcept
{
    m_move_direction = direction;
}

const movement_direction& movement::get_move_direction() const noexcept
{
    return m_move_direction;
}

graphics::graphics( const QString& image_path, bool visible ) :
    m_image_path( image_path ),
    m_visible( visible ){}

void graphics::set_image_path( const QString& image_path )
{
    m_image_path = image_path;
}

const QString& graphics::get_image_path() const noexcept
{
    return m_image_path;
}

void graphics::set_visible( bool visible ) noexcept
{
    m_visible = visible;
}

bool graphics::get_visible() const noexcept
{
    return m_visible;
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

}// component

}// ecs
