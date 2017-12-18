#ifndef SYSTEMS_H
#define SYSTEMS_H

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
    void tick() override;
    void clean() override;

private:
    component::geometry* m_map_geom{ nullptr };
};

class projectile_system final : public ecs::system
{
public:
    projectile_system( const QSize& projectile_size,
                       uint32_t projectile_damage,
                       uint32_t projectile_speed,
                       ecs::world& world ) noexcept;

    void init() override;
    void tick() override;
    void clean() override;

private:
    void create_explosion( const component::geometry& obstacle_geom );
    void handle_obstacle( ecs::entity& obstacle,
                          const component::projectile& projectile_comp );

    void kill_entity( ecs::entity& entity );

    void handle_existing_projectiles();
    void create_new_projectiles();

private:
    QSize m_projectile_size{};
    uint32_t m_damage{ 0 };
    uint32_t m_speed{ 0 };

    component::geometry* m_map_geom{ nullptr };
};

class respawn_system final : public ecs::system,
                             public ecs::event_callback< event::enemy_killed >,
                             public ecs::event_callback< event::player_killed >

{
    using clock = std::chrono::high_resolution_clock;
    struct death_info final
    {
        ecs::entity* entity;
        clock::time_point death_time;
    };

public:
    respawn_system( const std::chrono::milliseconds respawn_delay, ecs::world& world );
    ~respawn_system() override;

    void tick() override;
    void init() override;
    void clean() override;

    void on_event( const event::player_killed& );
    void on_event( const event::enemy_killed& );

private:
    void respawn_list( std::list< death_info >& list,
                       std::list< const component::geometry* > free_respawns );
    void respawn_entity( ecs::entity& entity, const component::geometry& respawn );
    std::list< const component::geometry* > get_free_respawns();

private:
    std::list< death_info > m_players_death_info;
    std::list< death_info > m_enemies_death_info;
    std::list< const component::geometry* > m_respawn_points;

    std::chrono::milliseconds m_respawn_delay{ 0 };
};

class win_defeat_system final : public ecs::system,
                                public ecs::event_callback< event::enemy_killed >,
                                public ecs::event_callback< event::player_killed >,
                                public ecs::event_callback< event::player_base_killed >

{
public:
    win_defeat_system( ecs::world& world ) noexcept;
    ~win_defeat_system();

    void init() override;
    void tick() override;
    void clean() override;

    void on_event( const event::enemy_killed& );
    void on_event( const event::player_killed& );
    void on_event( const event::player_base_killed& );

private:
    component::level_info* m_level_info{ nullptr };
    std::vector< ecs::entity* > m_frag_entities;
};

class tank_ai_system final : public ecs::system
{
public:
    explicit tank_ai_system( float chance_to_fire, ecs::world& world ) noexcept;
    void tick() override;
    void clean() override;

private:
    bool maybe_fire();

private:
    std::list< ecs::entity* > m_enemies;
    float m_chance_to_fire{ 0.0 };
};

class explosion_system final : public ecs::system,
                               public ecs::event_callback< event::projectile_collision >,
                               public ecs::event_callback< event::explosion_ended >
{
public:
    explicit explosion_system( ecs::world& world ) noexcept;
    ~explosion_system();

    void tick() override;

    void on_event( const event::projectile_collision& );
    void on_event( const event::explosion_ended& );
};

}// system

}// ecs

#endif
