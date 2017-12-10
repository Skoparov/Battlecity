#include "movable_map_object.h"

#include "ecs/components.h"

static constexpr auto str_move_direction_up = "Up";
static constexpr auto str_move_direction_down = "Down";
static constexpr auto str_move_direction_left = "Left";
static constexpr auto str_move_direction_right = "Right";
static constexpr auto str_move_direction_none = "None";

namespace game
{

movement_direction str_to_move_direction( const QString& direction_str )
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
    else if( direction_str != str_move_direction_none )
    {
        throw std::invalid_argument{ "Invalid movement string" };
    }

    return direction;
}

QString move_direction_to_str( const movement_direction& direction )
{
    QString direction_str{ str_move_direction_none };

    switch( direction )
    {
    case movement_direction::up: direction_str = str_move_direction_up; break;
    case movement_direction::down: direction_str = str_move_direction_down; break;
    case movement_direction::left: direction_str = str_move_direction_left; break;
    case movement_direction::right: direction_str = str_move_direction_right; break;
    case movement_direction::none: direction_str = str_move_direction_none; break;
    default: throw std::invalid_argument{ "Unimplemented movement_direction" }; break;
    }

    return direction_str;
}

movable_map_object::movable_map_object( ecs::entity* entity,
                                        const object_type& type,
                                        QObject* parent ):
    graphics_map_object( entity, type, parent ){}

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
