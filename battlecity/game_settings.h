#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include <map>
#include <chrono>

#include <QSize>
#include <QString>

#include "ecs/general_enums.h"

namespace game
{

class game_settings
{
public:
    void set_fps( const uint32_t fps ) noexcept;
    uint32_t get_fps() const noexcept;

    void set_tile_size( const QSize& size ) noexcept;
    const QSize& get_tile_size() const noexcept;

    void set_tank_size( const QSize& size ) noexcept;
    const QSize& get_tank_size() const noexcept;

    void set_projectile_size( const QSize& size ) noexcept;
    const QSize& get_projectile_size() const noexcept;

    void set_player_base_size( const QSize& size ) noexcept;
    const QSize& get_player_base_size() const noexcept;

    void set_tank_health( const uint32_t health ) noexcept;
    uint32_t get_tank_health() const noexcept;

    void set_wall_health( const uint32_t health ) noexcept;
    uint32_t get_wall_health() const noexcept;

    void set_iron_wall_health( const uint32_t health ) noexcept;
    uint32_t get_iron_wall_health() const noexcept;

    void set_player_base_health( const uint32_t health ) noexcept;
    uint32_t get_player_base_health() const noexcept;

    void set_player_lives( const uint32_t lives ) noexcept;
    uint32_t get_player_lives() const noexcept;

    void set_enemies_number( const uint32_t number ) noexcept;
    uint32_t get_enemies_number() const noexcept;

    void set_respawn_delay_ms( const uint32_t delay ) noexcept;
    uint32_t get_respawn_delay_ms() const noexcept;

    void set_base_kills_to_win( const uint32_t kills ) noexcept;
    uint32_t get_base_kills_to_win() const noexcept;

    void set_tank_speed( uint32_t speed ) noexcept;
    uint32_t get_tank_speed() const noexcept;

    void set_projectile_speed( uint32_t speed ) noexcept;
    uint32_t get_projectile_speed() const noexcept;

    void set_projectile_damage( uint32_t damage ) noexcept;
    uint32_t get_projectile_damage() const noexcept;

    void set_turret_cooldown_ms( uint32_t cooldown_ms ) noexcept;
    uint32_t get_turret_cooldown_ms() const noexcept;

    void set_ai_chance_to_fire( float chance_to_fire ) noexcept;
    float get_ai_chance_to_fire() const noexcept;

    void set_animation_data( const animation_type& type, const animation_data& data );
    const animation_data& get_animation_data( const animation_type& type ) const;
    const std::map< animation_type, animation_data >& get_animation_data() const noexcept;

private:
    uint32_t m_fps{ 0 };
    QSize m_tile_size{};
    QSize m_tank_size{};
    QSize m_projectile_size{};
    QSize m_player_base_size{};
    uint32_t m_tank_health{ 0 };
    uint32_t m_wall_health{ 0 };
    uint32_t m_iron_wall_health{ 0 };
    uint32_t m_base_health{ 0 };
    uint32_t m_player_lives{ 0 };
    uint32_t m_enemies_number{ 0 };
    uint32_t m_respawn_delay_ms{ 0 };
    uint32_t m_base_kills_to_win{ 0 };
    uint32_t m_tank_speed{ 0 };
    uint32_t m_projectile_speed{ 0 };
    uint32_t m_projectile_damage{ 0 };
    uint32_t m_turret_cooldown_ms{ 0 };
    float m_ai_chance_to_file{ 0.0 };

    std::map< animation_type, animation_data > m_animation_data;
};

game_settings read_game_settings( const QString& file );


}// game

#endif // GAME_SETTINGS_H
