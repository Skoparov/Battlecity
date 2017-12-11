#ifndef MAPDATA_H
#define MAPDATA_H

#include <type_traits>

#include <QSet>
#include <QSize>
#include <QString>
#include <QQmlListProperty>

#include "ecs/framework/world.h"

#include "map_objects/tile_map_object.h"
#include "map_objects/tank_map_object.h"

namespace std
{

template<> struct hash< game::object_type >
{
    using argument_type = game::object_type;
    using underlying_type = std::underlying_type< argument_type >::type;
    using result_type = std::hash< underlying_type >::result_type;

    size_t operator()( const argument_type& arg ) const
    {
        std::hash< underlying_type > hasher;
        return hasher( static_cast< underlying_type >( arg ) );
    }
};

}// std

inline uint qHash( const game::object_type& arg, uint seed )
{
    using underlying_type = std::underlying_type< game::object_type >::type;
    return qHash( static_cast< underlying_type >( arg ), seed );
}

namespace game
{

namespace detail
{

// Compile-time object_type enum to actual object type conversion
template< object_type > struct object_type_to_type;
template<> struct object_type_to_type< object_type::tile >{ using type = tile_map_object; };
template<> struct object_type_to_type< object_type::player_base >{ using type = graphics_map_object; };
template<> struct object_type_to_type< object_type::player_tank >{ using type = tank_map_object; };
template<> struct object_type_to_type< object_type::projectile >{ using type = movable_map_object; };

}// detail

class map_data final
{
private:
    template< object_type type > using object_ptr =
    typename std::add_pointer< typename detail::object_type_to_type< type >::type >::type;

public:
    map_data() = default;
    map_data( const QSize& map_size ) noexcept;

    void set_map_size( const QSize& size ) noexcept;
    void add_object( std::unique_ptr< base_map_object >&& object );

    QSet< object_type > remove_objects_from_active(
            const std::unordered_set< ecs::entity_id >& entities );

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;
    const QSize& get_map_size() const noexcept;

    template< object_type type >
    QList< object_ptr< type > > get_objects_of_type() const
    {
        using obj_ptr_type = object_ptr< type >;
        QList< obj_ptr_type > result_objects;

        auto eq_range = m_active_map_objects.equal_range( type );
        for( auto it = eq_range.first; it != eq_range.second; ++it )
        {
            result_objects << dynamic_cast< obj_ptr_type >( it->second.get() );
        }

        return result_objects;
    }

    void clear_inactive_objects(){ m_inactive_objects.clear(); }

private:
    QSize m_map_size{};
    std::unordered_multimap< object_type, std::unique_ptr< base_map_object > > m_active_map_objects;
    std::list< std::unique_ptr< base_map_object > > m_inactive_objects;
};

class game_settings;
map_data read_map_file(const QString& file, const game_settings& settings, ecs::world& world );

}// game

#endif // MAPDATA_H
