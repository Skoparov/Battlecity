#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <map>
#include <list>
#include <unordered_set>

#include <QRect>
#include <QString>

#include "ecs/framework/entity.h"
#include "general_enums.h"
#include "map_graph.h"

namespace game
{

namespace component
{

class positioning final
{
public:
    positioning() = default;
    explicit positioning( map_tile_node& initial_node );


    void add_node( map_tile_node& node );
    void remove_node( map_tile_node& node );
    std::unordered_set< map_tile_node* >& get_nodes() noexcept;
    const std::unordered_set< map_tile_node* >& get_nodes() const noexcept;

private:
    std::unordered_set< map_tile_node* > m_nodes;
};

//

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

class turret_object final : public ecs::rw_lock
{
    using clock = std::chrono::high_resolution_clock;

public:
    template< typename rep, typename period >
    turret_object( const std::chrono::duration< rep, period >& cooldown ) noexcept:
        m_cooldown( std::chrono::duration_cast< std::chrono::milliseconds >( cooldown ) ){}

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

class animation final{};

//

class power_up final
{
public:
    enum class state{ active, waiting_to_respawn };

public:
    explicit power_up( const powerup_type& type ) noexcept;

    void set_state( const state& state ) noexcept;

    const powerup_type& get_type() const noexcept;
    const state& get_state() const noexcept;

private:
    powerup_type m_type;
    state m_state{ state::waiting_to_respawn };
};

//

class object_effects final
{
public:
    void add_effect( ecs::entity& e );
    std::list< ecs::entity* >& get_effects() noexcept;

private:
    std::list< ecs::entity* > m_effects;
};

//

class respawn_delay final
{
public:
    explicit respawn_delay( const std::chrono::milliseconds& delay ) noexcept;
    const std::chrono::milliseconds& get_respawn_delay() const noexcept;

private:
    std::chrono::milliseconds m_respawn_delay;
};


//

class frag final
{
public:
    explicit frag( uint32_t num ) noexcept;
    uint32_t get_num() const noexcept;

private:
    const uint32_t m_num{ 0 };
};

//

class projectile final
{
public:
    projectile() = default;
    projectile( uint32_t damage, const ecs::entity& owner );

    void set_damage( uint32_t damage ) noexcept;
    void set_destroyed() noexcept;

    uint32_t get_damage() const noexcept;
    bool get_destroyed() const noexcept;
    ecs::entity_id get_shooter_id() const noexcept;
    const object_type& get_shooter_type() const noexcept;

private:
    uint32_t m_damage{ 1 };
    ecs::entity_id m_shooter_id{ INVALID_NUMERIC_ID };
    object_type m_shooter_type;
    bool m_is_destroyed{ false };
};

//

class geometry final : public ecs::rw_lock
{
public:
    geometry() = default;
    geometry( const QRect& rect, int rotation = 0 ) noexcept;

    void move_center_to( const QPoint& pos ) noexcept;

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

class movement final : public ecs::rw_lock
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

class non_traversible_tile final{};
class non_traversible_object final{};
class flying final{}; // can pass through non_traversible

//

class graphics final : public ecs::rw_lock
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

class animation_info
{
public:
    animation_info() = default;

    template< typename rep, typename period >
    animation_info( const animation_type& type,
                    uint32_t frames_num,
                    uint32_t frame_rate,
                    uint32_t loops_num,
                    const std::chrono::duration< rep, period >& duration ):
           m_type( type ),
           m_frames_num( frames_num ),
           m_frame_rate( frame_rate ),
           m_loops_num( loops_num ),
           m_duration( std::chrono::duration_cast< std::chrono::milliseconds >( duration ) ){}

    void force_stop() noexcept;

    const animation_type& get_type() const noexcept;
    uint32_t get_frames_num() const noexcept;
    uint32_t get_frame_rate() const noexcept;
    uint32_t get_loops_num() const noexcept;
    const std::chrono::milliseconds& get_duration() const noexcept;
    bool is_infinite() const noexcept;

private:
    animation_type m_type;
    uint32_t m_frames_num{ 0 };
    uint32_t m_frame_rate{ 0 };
    uint32_t m_loops_num{ 0 };
    std::chrono::milliseconds m_duration{ 0 };
};

//

class health final : public ecs::rw_lock
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

class lifes final : public ecs::rw_lock
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

//

class respawn_point{};

//

class shield final
{
public:
    shield() = default;
    explicit shield( uint32_t max_health ) noexcept;

    void increase( uint32_t value ) noexcept;
    void decrease( uint32_t value ) noexcept;

    uint32_t get_shield_health() const noexcept;
    uint32_t get_max_shield_ehalth() const noexcept;

    bool has_shield() const noexcept;

private:
    uint32_t m_shield{ 0 };
    const uint32_t m_max_shield{ 0 };
};

//

class powerup_animations final
{
public:
    void add_animation( const powerup_type& type, ecs::entity& e );
    void remove_animation( const powerup_type& type );
    ecs::entity& get_animation( const powerup_type& type );
    bool has_animation( const powerup_type& type ) const;

    std::map< powerup_type, ecs::entity* >& get_animations() noexcept;

private:
    std::map< powerup_type, ecs::entity* > m_animations;
};

}// components

}// game

#endif
