#ifndef MAPDATA_H
#define MAPDATA_H

#include <type_traits>

#include <QSize>

#include "map_objects/base_map_object.h"

namespace ecs
{

class world;

}

namespace game
{

class map_data_mediator
{
public:
    virtual ~map_data_mediator() = default;
    virtual void add_object( const object_type& type, ecs::entity& entity ) = 0;
    virtual void level_changed() = 0;
    virtual void remove_all() = 0;
};

class map_data final
{
public:
    map_data() = default;
    map_data( const QSize& map_size ) noexcept;

    void set_map_size( const QSize& size ) noexcept;
    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;
    const QSize& get_map_size() const noexcept;

private:
    QSize m_map_size{};
};

class game_settings;
void read_map_file( map_data& data,
                    const QString& file,
                    const game_settings& settings,
                    ecs::world& world ,
                    map_data_mediator* mediator = nullptr );

}// game

#endif // MAPDATA_H
