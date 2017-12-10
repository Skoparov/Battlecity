#include "systems.h"

static constexpr int rotation_left{ 270 };
static constexpr int rotation_right{ 90 };
static constexpr int rotation_top{ 0 };
static constexpr int rotation_bottom{ 180 };

namespace game
{

namespace system
{

movement_system::movement_system( ecs::world& world ): ecs::system( world ){}

QRect calc_move( component::movement& move,
                 component::geometry& obj_geom,
                 component::geometry& map_geom ) noexcept
{
    int x_mult{ 0 };
    int y_mult{ 0 };
    int rotation{ 0 };

    const movement_direction& direction = move.get_move_direction();

    if( direction == movement_direction::left )
    {
        x_mult = -1;
        rotation = rotation_left;
    }
    else if( direction == movement_direction::right )
    {
        x_mult = 1;
        rotation = rotation_right;
    }
    else if( direction == movement_direction::up )
    {
        y_mult = -1;
        rotation = rotation_top;
    }
    else if( direction == movement_direction::down )
    {
        y_mult = 1;
        rotation = rotation_bottom;
    }

    QRect obj_rect{ obj_geom.get_rect() };

    uint32_t speed{ move.get_speed() };
    obj_rect.translate( x_mult * speed, y_mult * speed );

    obj_geom.set_rotation( rotation );

    QRect map_rect{ map_geom.get_rect() };
    if( obj_rect.left() < map_rect.left() )
    {
        obj_rect.moveLeft( map_rect.left() );
    }
    else if( obj_rect.right() > map_rect.right() )
    {
        obj_rect.moveRight( map_rect.right() );
    }

    if( obj_rect.top() < map_rect.top() )
    {
        obj_rect.moveTop( map_rect.top() );
    }
    if( obj_rect.bottom() > map_rect.bottom() )
    {
        obj_rect.moveBottom( map_rect.bottom() );
    }

    return obj_rect;
}

bool can_pass_through_non_traversible( ecs::entity& e )
{
    return e.has_component< component::projectile >();
}

void movement_system::tick()
{
    using namespace component;

    ecs::entity* map_entity{ m_world.entities_with_component< map_object >().front() };
    geometry& map_geom = map_entity->get_component_unsafe< geometry >();

    m_world.for_each< movement >( [ & ]( ecs::entity& curr_entity, movement& move )
    {
        if( move.get_move_direction() != movement_direction::none )
        {
            geometry& curr_geom = curr_entity.get_component_unsafe< geometry >();
            int prev_rotation{ curr_geom.get_rotation() };
            QRect rect_after_move{ calc_move( move, curr_geom, map_geom ) };
            bool movement_valid{ true };

            m_world.for_each< non_traversible >( [ & ]( ecs::entity& other_entity, non_traversible& )
            {
                if( curr_entity != other_entity )
                {
                    geometry& other_geom = other_entity.get_component_unsafe< geometry >();
                    if( other_geom.intersects_with( rect_after_move ) )
                    {
                        movement_valid = false;
                    }
                }

                return movement_valid;
            } );

            bool x_changed{ false };
            bool y_changed{ false };
            bool rotation_changed{ prev_rotation != curr_geom.get_rotation() };

            if( movement_valid )
            {
                x_changed = curr_geom.get_pos().x() != rect_after_move.x();
                y_changed = curr_geom.get_pos().y() != rect_after_move.y();
                curr_geom.set_pos( rect_after_move.topLeft() );
            }

            if( x_changed || y_changed || rotation_changed )
            {
                event::geometry_changed event{ x_changed, y_changed, rotation_changed };
                event.add_entity( curr_entity.get_id() );
                m_world.emit_event( event );
            }
        }

        return true;
    } );
}

}// system

}// game


