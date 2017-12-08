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
    void load_next_level();

    void start();
    void stop();

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;

    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;

    QList< tile_map_object* > get_tiles();

public slots:
    void tick();

private:
    game_settings m_settings;
    map_data m_map_data;
    ecs::world& m_world;
    QTimer* m_tick_timer{ nullptr };
    uint32_t m_level{ 0 };
};

}// game

#endif // CONTROLLER_H
