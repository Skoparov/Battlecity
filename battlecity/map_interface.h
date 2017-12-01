#ifndef MAPINTERFACE_H
#define MAPINTERFACE_H

#include <QObject>
#include <QQmlListProperty>

#include "map_data.h"

namespace game
{

class qml_map_interface : public QObject
{
    Q_OBJECT

public:
    qml_map_interface( ecs::world& world, QObject* parent = nullptr );
    ~qml_map_interface(){}

    void load_level( uint32_t level_num );
    void clear();

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;

    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;

    QQmlListProperty< tile_map_object > get_tiles();

    Q_PROPERTY( QQmlListProperty< tile_map_object > tiles READ get_tiles NOTIFY tiles_changed )
    Q_PROPERTY( int rows_num READ get_rows_count CONSTANT )
    Q_PROPERTY( int columns_num READ get_columns_count CONSTANT )
    Q_PROPERTY( int tile_width READ get_tile_width CONSTANT )
    Q_PROPERTY( int tile_height READ get_tile_height CONSTANT )

signals:
    void tiles_changed( QQmlListProperty< tile_map_object > );

private:
    map_data m_map_data;
    ecs::world& m_world;
};

}// game

#endif // MAPINTERFACE_H
