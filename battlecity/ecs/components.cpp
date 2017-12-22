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

bool turret_object::set_fire_status( bool fired ) noexcept
{
    using namespace std::chrono;

    bool result{ true };

    if( fired && !m_fired )
    {
        auto curr_time = clock::now();
        result = duration_cast< milliseconds >( curr_time - m_last_fired ) >= m_cooldown;
        if( result )
        {
            m_fired = true;
            m_last_fired = curr_time;
        }
    }
    else
    {
        m_fired = fired;
    }

    return result;
}

bool turret_object::has_fired() const noexcept
{
    return m_fired;
}

//

frag::frag( uint32_t num ) noexcept: m_num( num ){}

uint32_t frag::get_num() const noexcept
{
    return m_num;
}

//

projectile::projectile( uint32_t damage, const ecs::entity& owner ) :
    m_damage( damage ),
    m_shooter_id( owner.get_id() )
{
    if( owner.has_component< player >() )
    {
        m_shooter_type = object_type::player_tank;
    }
    else if( owner.has_component< enemy >() )
    {
        m_shooter_type = object_type::enemy_tank;
    }
    else
    {
        throw std::invalid_argument{ "Unimplemented owner type" };
    }
}

void projectile::set_damage( uint32_t damage ) noexcept
{
    m_damage = damage;
}

void projectile::set_destroyed() noexcept
{
    m_is_destroyed = true;
}

uint32_t projectile::get_damage() const noexcept
{
    return m_damage;
}

bool projectile::get_destroyed() const noexcept
{
    return m_is_destroyed;
}

ecs::entity_id projectile::get_shooter_id() const noexcept
{
    return m_shooter_id;
}

const object_type& projectile::get_shooter_type() const noexcept
{
    return m_shooter_type;
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

void geometry::set_rect( const QRect& rect ) noexcept
{
    m_rect = rect;
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

const animation_type& animation_info::get_type() const noexcept
{
    return m_type;
}

uint32_t animation_info::get_frames_num() const noexcept
{
    return m_frames_num;
}

uint32_t animation_info::get_frame_rate() const noexcept
{
    return m_frame_rate;
}

uint32_t animation_info::get_loops_num() const noexcept
{
    return m_loops_num;
}

const std::chrono::milliseconds& animation_info::get_duration() const noexcept
{
    return m_duration;
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

//

lifes::lifes( const has_infinite_lifes& mode, uint32_t lifes ) noexcept :
    m_mode( mode ),
    m_lifes( lifes ){}

void lifes::increase( uint32_t value ) noexcept
{
    if( m_mode != has_infinite_lifes::yes )
    {
        m_lifes += value;
    }
}

void lifes::decrease( uint32_t value ) noexcept
{
    if( m_mode != has_infinite_lifes::yes )
    {
        m_lifes = ( m_lifes >= value) ? m_lifes - value : 0;
    }
}

bool lifes::has_life() const noexcept
{
    return ( m_mode == has_infinite_lifes::yes || m_lifes != 0 );
}

uint32_t lifes::get_lifes() const noexcept
{
    return m_lifes;
}

const has_infinite_lifes& lifes::get_mode() const noexcept
{
    return m_mode;
}

//

void kills_counter::increase( uint32_t value ) noexcept
{
    m_kills += value;
}

uint32_t kills_counter::get_kills() const noexcept
{
    return m_kills;
}

}// component

}// ecs
