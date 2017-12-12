#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "events.h"
#include "components.h"
#include "framework/world.h"

namespace game
{

namespace system
{

class movement_system : public ecs::system
{
public:
    explicit movement_system( ecs::world& world );
    void tick() override;
};

class projectile_system : public ecs::system
{
public:
    projectile_system( const QSize& projectile_size,
                       uint32_t projectile_damage,
                       uint32_t projectile_speed,
                       ecs::world& world ) noexcept;

    void tick() override;

private:
    void handle_obstacle( ecs::entity& obstacle,
                          const component::projectile& projectile_comp,
                          event::entities_removed& event );

    void handle_existing_projectiles();
    void create_new_projectiles();

private:
    QSize m_projectile_size{};
    uint32_t m_damage{ 0 };
    uint32_t m_speed{ 0 };
};

class respawn_system : public ecs::system,
        public ecs::event_callback< event::enemy_killed >,
        public ecs::event_callback< event::player_killed >

{
public:
    explicit respawn_system( ecs::world& world ) noexcept;

    void tick() override;
    void clean() override;

    void on_event( const event::player_killed& );
    void on_event( const event::enemy_killed& );

private:
    bool m_player_needs_respawn{ false };
    bool m_enemy_needs_respawn{ false };
};

class win_defeat_system : public ecs::system,
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

}// system

}// ecs

#endif
