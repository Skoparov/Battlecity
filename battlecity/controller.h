#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QTimer>

#include "map_data.h"
#include "game_settings.h"

namespace game
{

class controller : public QObject
{
    Q_OBJECT

public:
    controller( const game_settings& settings, ecs::world& world );
    void init();

    void start();
    void stop();

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;

    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;

    QList< tile_map_object* > get_tiles() const;
    QList< graphics_map_object* > get_player_bases() const;
    QList< tank_map_object* > get_player_tanks() const;

private:
    void load_next_level();

public slots:
    void tick();

private:
    game_settings m_settings;
    ecs::world& m_world;

    map_data m_map_data;
    uint32_t m_level{ 0 };
    QTimer* m_tick_timer{ nullptr };
    std::list< std::unique_ptr< ecs::system > > m_systems;
};

}// game

#endif // CONTROLLER_H
