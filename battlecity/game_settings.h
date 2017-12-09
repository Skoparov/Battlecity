#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include <QSize>
#include <QString>

namespace game
{

class game_settings
{
public:
    void set_tile_size( const QSize& size ) noexcept;
    const QSize& get_tile_size() const noexcept;

    void set_base_health( const uint32_t health ) noexcept;
    uint32_t get_base_health() const noexcept;

    void set_player_health( const uint32_t health ) noexcept;
    uint32_t get_player_health() const noexcept;

    void set_base_kills_to_win( const uint32_t kills ) noexcept;
    uint32_t get_base_kills_to_win() const noexcept;

    void set_fps( const uint32_t fps ) noexcept;
    uint32_t get_fps() const noexcept;

    void set_tank_movement_speed( uint32_t speed ) noexcept;
    uint32_t get_tank_movement_speed() const noexcept;

private:
    QSize m_tile_size{};
    uint32_t m_fps{ 0 };
    uint32_t m_base_health{ 0 };
    uint32_t m_player_health{ 0 };
    uint32_t m_base_kills_to_win{ 0 };
    uint32_t m_tank_movement_speed{ 0 }; // TODO
};

game_settings read_game_settings( const QString& file );


}// game

#endif // GAME_SETTINGS_H
