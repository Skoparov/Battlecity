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

class respawn_system final : public ecs::system,
                             public ecs::event_callback< event::entity_killed >

{
    using clock = std::chrono::high_resolution_clock;
    struct death_info final
    {
        ecs::entity* entity;
        clock::time_point death_time;
    };

public:
    template< typename rep, typename period >
    respawn_system( const std::chrono::duration< rep, period >& respawn_delay,
                    ecs::world& world ):
        ecs::system( world ),
        m_respawn_delay( std::chrono::duration_cast< std::chrono::milliseconds >( respawn_delay ) )
    {
        m_world.subscribe< event::entity_killed >( *this );
    }

    ~respawn_system() override;

    void tick() override;
    void init() override;
    void clean() override;

    void on_event( const event::entity_killed& );

private:
    void respawn_is_ready( std::list< death_info >& list,
                       std::vector< const component::geometry* > free_respawns );
    void respawn_entity( ecs::entity& entity, const component::geometry& respawn );
    std::vector< const component::geometry* > get_free_respawns();

private:
    std::list< death_info > m_players_death_info;
    std::list< death_info > m_enemies_death_info;
    std::list< const component::geometry* > m_respawn_points;

    std::chrono::milliseconds m_respawn_delay{ 0 };
};

class win_defeat_system final : public ecs::system,
                                public ecs::event_callback< event::entity_killed >

{
public:
    win_defeat_system( uint32_t kills_to_win, ecs::world& world ) noexcept;
    ~win_defeat_system();

    void init() override;
    void tick() override;
    void clean() override;

    void on_event( const event::entity_killed& );

private:
    uint32_t m_kills_to_win{ 0 };

    ecs::entity* m_player{ nullptr };
    ecs::entity* m_player_base{ nullptr };
    std::vector< ecs::entity* > m_frag_entities;
};

class tank_ai_system final : public ecs::system
{
public:
    explicit tank_ai_system( float chance_to_fire, ecs::world& world ) noexcept;
    void init();
    void tick() override;
    void clean() override;

private:
    bool maybe_fire();

private:
    ecs::entity* m_player{ nullptr };
    std::list< ecs::entity* > m_enemies;
    float m_chance_to_fire{ 0.0 };
};

class animation_system final : public ecs::system,
                               public ecs::event_callback< event::projectile_collision >,
                               public ecs::event_callback< event::entity_respawned >
{
private:
    using clock = std::chrono::high_resolution_clock;

    struct animation_info
    {
        ecs::entity* entity;
        clock::time_point start;
    };

public:
    explicit animation_system( ecs::world& world ) noexcept;
    ~animation_system();

    void clean();
    void tick() override;

    void add_animation_settings( const animation_type& type, const animation_data& data );

    void on_event( const event::projectile_collision& );
    void on_event( const event::entity_respawned& );

private:
    void create_animation_entity( const QRect& rect, const animation_type& type );

private:
    std::list< animation_info > m_animations;
    std::map< animation_type, animation_data > m_animation_data;
};
}// system

}// ecs

#endif
