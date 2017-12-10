#include "movable_map_object.h"

#include "ecs/components.h"

static constexpr auto str_move_direction_up = "Up";
static constexpr auto str_move_direction_down = "Down";
static constexpr auto str_move_direction_left = "Left";
static constexpr auto str_move_direction_right = "Right";

namespace game
{

movement_direction str_to_move_direction( const QString& direction_str ) noexcept
{
    movement_direction direction{ movement_direction::none };

    if( direction_str == str_move_direction_up )
    {
        direction = movement_direction::up;
    }
    else if( direction_str == str_move_direction_down )
    {
        direction = movement_direction::down;
    }
    else if( direction_str == str_move_direction_left )
    {
        direction = movement_direction::left;
    }
    else if( direction_str == str_move_direction_right )
    {
        direction = movement_direction::right;
    }

    return direction;
}

QString move_direction_to_str( const movement_direction& direction )
{
    QString direction_str{ "None" };

    if( direction == movement_direction::up )
    {
        direction_str = str_move_direction_up;
    }
    else if( direction == movement_direction::down )
    {
        direction_str = str_move_direction_down;
    }
    else if( direction == movement_direction::left )
    {
        direction_str = str_move_direction_left;
    }
    else if( direction == movement_direction::right )
    {
        direction_str = str_move_direction_right;
    }

    return direction_str;
}

movable_map_object::movable_map_object( ecs::entity* entity,
                                        const object_type& type,
                                        QObject* parent ):
    base_map_object( entity, type, parent ){}

void movable_map_object::set_move_direction( const QString& direction )
{
    component::movement& m = m_entity->get_component_unsafe< component::movement >();
    m.set_move_direction( str_to_move_direction( direction ) );
}

QString movable_map_object::get_move_direction() const
{
    component::movement& m = m_entity->get_component_unsafe< component::movement >();
    return move_direction_to_str( m.get_move_direction() );
}

}// game
