#ifndef MAPDATA_H
#define MAPDATA_H

#include <QSize>
#include <QString>

#include "ecs/framework/world.h"


namespace game
{

class map_data final
{
public:
    map_data() = default;
    map_data(const QSize& map_size, const QSize& tile_size, ecs::world* world ) noexcept;

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;
    const QSize& get_map_size() const noexcept;

    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;
    const QSize& get_tile_size() const noexcept;

private:
    QSize m_map_size{};
    QSize m_tile_size{};

    ecs::world* m_world{ nullptr };
};


map_data read_map_file(const QString& file, ecs::world& world );

}// game

#endif // MAPDATA_H
