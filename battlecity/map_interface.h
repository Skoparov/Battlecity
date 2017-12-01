#ifndef MAPINTERFACE_H
#define MAPINTERFACE_H

#include <QObject>
#include <QQmlListProperty>

#include "map_data.h"

namespace game
{

class map_interface : public QObject
{
public:
    map_interface( ecs::world& world, QObject* parent = nullptr );

    void load_level( uint32_t level_num );
    void clear();

    int get_rows_num() const noexcept;
    int get_columns_num() const noexcept;

    Q_PROPERTY( uint32_t rows_num READ get_rows_num CONSTANT )
    Q_PROPERTY( uint32_t columns_num READ get_columns_num CONSTANT )

private:
    map_data m_map_data;
    ecs::world& m_world;
};

}// game

#endif // MAPINTERFACE_H
