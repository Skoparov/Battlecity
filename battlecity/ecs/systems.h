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
    void tick() override;
};

class projectile_system final : public ecs::system
{
public:
    projectile_system( const QSize& projectile_size,
                       uint32_t projectile_damage,
                       uint32_t projectile_speed,
                       ecs::world& world ) noexcept;

    void tick() override;

private:
    void handle_obstacle( ecs::entity& obstacle,
                          const component::projectile& projectile_comp );

    void kill_entity( ecs::entity& entity );

    void handle_existing_projectiles();
    void create_new_projectiles();

private:
    QSize m_projectile_size{};
    uint32_t m_damage{ 0 };
    uint32_t m_speed{ 0 };
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

    ~respawn_system();

    void tick() override;
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
    win_defeat_system( uint32_t kills_to_win, uint32_t player_lifes, ecs::world& world ) noexcept;
    ~win_defeat_system();

    void tick() override;
    void clean() override;

    void on_event( const event::enemy_killed& );
    void on_event( const event::player_killed& );
    void on_event( const event::player_base_killed& );

private:
    uint32_t m_kills_to_win{ 0 };
    uint32_t m_player_lifes{ 0 };

    bool m_player_base_killed{ false };
    uint32_t m_player_kills{ 0 };
    uint32_t m_player_lifes_left{ 0 };
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

}// system

}// ecs

#endif
