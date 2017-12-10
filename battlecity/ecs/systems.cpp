#include "systems.h"

static constexpr int rotation_left{ 270 };
static constexpr int rotation_right{ 90 };
static constexpr int rotation_top{ 0 };
static constexpr int rotation_bottom{ 180 };

namespace game
{

using namespace component;

namespace systems
{

movement_system::movement_system( ecs::world& world ): ecs::system( world ){}

QRect calc_rect_after_move( movement& move,
                            geometry& obj_geom,
                            geometry& map_geom ) noexcept
{
    QRect obj_rect{ obj_geom.get_rect() };

    if( move.get_move_direction() == movement_direction::left )
    {
        obj_rect.moveLeft( move.get_speed() );
        obj_geom.set_rotation( rotation_left );
    }
    else if( move.get_move_direction() == movement_direction::right )
    {
        obj_rect.moveRight( move.get_speed() );
        obj_geom.set_rotation( rotation_right );
    }
    else if( move.get_move_direction() == movement_direction::up )
    {
        obj_rect.moveTop( move.get_speed() );
        obj_geom.set_rotation( rotation_top );
    }
    else if( move.get_move_direction() == movement_direction::down )
    {
        obj_rect.moveBottom( move.get_speed() );
        obj_geom.set_rotation( rotation_bottom );
    }

    QRect map_rect{ map_geom.get_rect() };
    if( obj_rect.left() < map_rect.left() )
    {
        obj_rect.setLeft( map_rect.left() );
    }
    else if( obj_rect.right() > map_rect.right() )
    {
        obj_rect.setRight( map_rect.right() );
    }

    if( obj_rect.top() < map_rect.top() )
    {
        obj_rect.setTop( map_rect.top() );
    }
    if( obj_rect.bottom() > map_rect.bottom() )
    {
        obj_rect.setBottom( map_rect.bottom() );
    }

    return obj_rect;
}

bool can_pass_through_non_traversible( ecs::entity& e )
{
    return e.has_component< projectile >();
}

void movement_system::tick()
{
    ecs::entity* map_entity{ m_world.entities_with_component< map_object >().front() };
    geometry& map_geom = map_entity->get_component_unsafe< geometry >();

    std::list< ecs::entity* > entities_with_movement{
        m_world.entities_with_component< movement >() };

    std::list< ecs::entity* > non_traversable_entities{
        m_world.entities_with_component< non_traversible >() };

    for( ecs::entity* curr_entity : entities_with_movement )
    {
        geometry& curr_geom = curr_entity->get_component_unsafe< geometry >();
        movement& move = curr_entity->get_component_unsafe< movement >();

        if( move.get_move_direction() != movement_direction::none )
        {
            QRect rect_after_move{ calc_rect_after_move( move, curr_geom, map_geom ) };
            bool movement_valid{ true };

            if( !can_pass_through_non_traversible( *curr_entity ) )
            {
                for( ecs::entity* other_entity : non_traversable_entities )
                {
                    if( curr_entity != other_entity )
                    {
                        geometry& other_geom = other_entity->get_component_unsafe< geometry >();
                        if( other_geom.intersects_with( rect_after_move ) )
                        {
                            movement_valid = false;
                            break;
                        }
                    }
                }
            }

            if( movement_valid )
            {
                curr_geom.set_pos( rect_after_move.topLeft() );
            }

            move.set_move_direction( movement_direction::none );
        }
    }
}

}// systems

}// game


