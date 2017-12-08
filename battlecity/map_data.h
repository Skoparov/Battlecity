#ifndef MAPDATA_H
#define MAPDATA_H

#include <list>
#include <type_traits>

#include <QSize>
#include <QString>
#include <QQmlListProperty>

#include "ecs/framework/world.h"

#include "map_objects/tile_map_object.h"

namespace game
{

namespace detail
{

// Compile-time object_type enum to actual object type conversion
template< object_type > struct object_type_to_type;
template<> struct object_type_to_type< object_type::tile >{ using type = tile_map_object; };

}// detail

class map_data final
{
private:
    template< object_type type >
    using object_ptr =
    typename std::add_pointer< typename detail::object_type_to_type< type >::type >::type;

public:
    map_data() = default;
    map_data( const QSize& map_size,
              std::list< std::unique_ptr< base_map_object > >&& objects ) noexcept;

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;
    const QSize& get_map_size() const noexcept;

    template< object_type type >
    auto get_objects_of_type() const ->
    QList< object_ptr< type > >
    {
        using obj_ptr_type = object_ptr< type >;
        QList< obj_ptr_type > result_objects;

        for( const auto& object : m_map_objects )
        {
            if( object->get_type() == type )
            {
                result_objects << dynamic_cast< obj_ptr_type >( object.get() );
            }
        }

        return result_objects;
    }

private:
    QSize m_map_size{};
    std::list< std::unique_ptr< base_map_object > > m_map_objects;
};

class game_settings;
map_data read_map_file(const QString& file, const game_settings& settings, ecs::world& world );

}// game

#endif // MAPDATA_H
