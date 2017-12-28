#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <map>

#include "events.h"
#include "components.h"
#include "framework/world.h"

namespace game
{

namespace system
{

class movement_system final : public ecs::system
{
public:
    explicit movement_system( ecs::world& world );

    void init() override;
    bool tick() override;
    void clean() override;

private:
    component::geometry* m_map_geom{ nullptr };
};

//

class projectile_system final : public ecs::system
{
public:
    projectile_system( const QSize& projectile_size,
                       uint32_t projectile_damage,
                       uint32_t projectile_speed,
                       ecs::world& world ) noexcept;

    void init() override;
    bool tick() override;
    void clean() override;

private:
    void create_explosion( const component::geometry& obstacle_geom );
    void handle_obstacle( ecs::entity& obstacle,
                          const component::projectile& projectile_comp );

    void kill_entity( ecs::entity& victim,
                      const object_type& victim_type,
                      ecs::entity* killer,
                      const object_type& killer_type );

    void handle_existing_projectiles();
    void create_new_projectiles();

private:
    QSize m_projectile_size{};
    uint32_t m_damage{ 0 };
    uint32_t m_speed{ 0 };

    component::geometry* m_map_geom{ nullptr };
};

//

class respawn_system final : public ecs::system,
                             public ecs::event_callback< event::entity_killed >,
                             public ecs::event_callback< event::powerup_taken >

{
    using clock = std::chrono::high_resolution_clock;
    struct death_info final
    {
        ecs::entity* entity;
        clock::time_point death_time;
    };

public:
    explicit respawn_system( ecs::world& world ) noexcept;
    ~respawn_system() override;

    bool tick() override;
    void init() override;
    void clean() override;

    void on_event( const event::entity_killed& );
    void on_event( const event::powerup_taken& );

private:
    void maybe_add_to_respawn_list( ecs::entity& e );
    void respawn_if_ready( std::list< death_info >& list,
                           std::vector< const component::geometry* > free_respawns );
    void respawn_entity( ecs::entity& entity, const component::geometry& respawn );

    std::vector< const component::geometry* > get_free_respawns();

private:
    std::list< death_info > m_death_info;
    std::vector< const component::geometry* > m_empty_tiles;
};

//

class powerup_system final : public ecs::system
{
public:
    explicit powerup_system( ecs::world& world ) noexcept;

    bool tick() override;

private:
    void apply_powerup( const powerup_type& type, ecs::entity& target );
    void deactivate_powerup( ecs::entity& powerup,
                             component::power_up& comp,
                             ecs::entity& taker );


};

//

class win_defeat_system final : public ecs::system,
                                public ecs::event_callback< event::entity_killed >

{
public:
    win_defeat_system( uint32_t kills_to_win, ecs::world& world ) noexcept;
    ~win_defeat_system();

    void init() override;
    bool tick() override;
    void clean() override;

    void on_event( const event::entity_killed& );

private:
    uint32_t m_kills_to_win{ 0 };

    ecs::entity* m_player{ nullptr };
    ecs::entity* m_player_base{ nullptr };
    std::vector< ecs::entity* > m_frag_entities;
};

//

class tank_ai_system final : public ecs::system
{
public:
    explicit tank_ai_system( float chance_to_fire,
                             float chance_to_change_direction,
                             ecs::world& world ) noexcept;
    void init();
    bool tick() override;
    void clean() override;

private:
    bool maybe_fire();
    bool maybe_change_direction();
    bool make_decision( float chance ) const;

private:
    ecs::entity* m_player{ nullptr };
    std::list< ecs::entity* > m_enemies;
    float m_chance_to_fire{ 0.0 };
    float m_chance_to_change_direction{ 0.0 };
};

//

class animation_system final : public ecs::system,
                               public ecs::event_callback< event::projectile_collision >,
                               public ecs::event_callback< event::entity_respawned >,
                               public ecs::event_callback< event::powerup_taken >
{
private:
    using clock = std::chrono::high_resolution_clock;

    struct animation_start_info
    {
        ecs::entity* entity;
        clock::time_point start;
    };

public:
    explicit animation_system( ecs::world& world ) noexcept;
    ~animation_system();

    void clean();
    bool tick() override;

    void add_animation_settings( const animation_type& type, const animation_data& data );

    void on_event( const event::projectile_collision& );
    void on_event( const event::entity_respawned& );
    void on_event( const event::powerup_taken& );

private:
    ecs::entity& create_animation_entity( const QRect& rect, const animation_type& type );

private:
    std::list< animation_start_info > m_animations;
    std::map< animation_type, animation_data > m_animation_data;
};
}// system

}// ecs

#endif
