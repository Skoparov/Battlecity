#include "systems.h"

#include "entity_factory.h"

static constexpr int rotation_left{ 270 };
static constexpr int rotation_right{ 90 };
static constexpr int rotation_top{ 0 };
static constexpr int rotation_bottom{ 180 };

namespace game
{

namespace system
{

movement_system::movement_system( ecs::world& world ): ecs::system( world ){}

bool can_pass_through_non_traversible( ecs::entity& e )
{
    return e.has_component< component::projectile >();
}

QRect calc_move( component::movement& move,
                 component::geometry& obj_geom,
                 component::geometry& map_geom,
                 bool can_pass_though_non_traversible ) noexcept
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

    if( !can_pass_though_non_traversible )
    {
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
    }

    return obj_rect;
}

void movement_system::tick()
{
    using namespace component;

    geometry* map_geom{ nullptr };

    m_world.for_each< movement >( [ & ]( ecs::entity& curr_entity, movement& move )
    {
        if( move.get_move_direction() != movement_direction::none )
        {
            if( !map_geom )
            {
                ecs::entity* map_entity{ m_world.get_entities_with_component< game_map >().front() };
                map_geom = &map_entity->get_component_unsafe< geometry >();
            }

            geometry& curr_geom = curr_entity.get_component_unsafe< geometry >();
            int prev_rotation{ curr_geom.get_rotation() };
            bool can_pass_though_everything{ can_pass_through_non_traversible( curr_entity ) };
            bool movement_valid{ true };

            QRect rect_after_move{ calc_move( move, curr_geom, *map_geom, can_pass_though_everything ) };

            if( !can_pass_though_everything )
            {
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
            }

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

movement_direction get_direction_by_rotation( int rotation )
{
    movement_direction direction{ movement_direction::none };

    switch( rotation )
    {
    case rotation_left: direction = movement_direction::left; break;
    case rotation_right: direction = movement_direction::right; break;
    case rotation_top: direction = movement_direction::up; break;
    case rotation_bottom: direction = movement_direction::down; break;
    default: throw std::invalid_argument{ "Invalid rotation value" }; break;
    }

    return direction;
}

projectile_system::projectile_system( ecs::world& world ): ecs::system( world ){}

void projectile_system::tick()
{
    using namespace component;

    // Calc existing projectiles
    geometry* map_geom{ nullptr };
    event::entities_removed entities_removed_event;

    m_world.for_each< projectile >( [ & ]( ecs::entity& projectile_entity, projectile& projectile_component )
    {
        if( !map_geom )
        {
            ecs::entity* map_entity{ m_world.get_entities_with_component< game_map >().front() };
            map_geom = &map_entity->get_component_unsafe< geometry >();
        }

        geometry& projectile_geom = projectile_entity.get_component_unsafe< geometry >();
        bool projectile_destroyed{ false };

        if( !map_geom->intersects_with( projectile_geom ) )
        {
            projectile_destroyed = true;
        }
        else
        {
            m_world.for_each< non_traversible >( [ & ]( ecs::entity& obstacle, non_traversible& )
            {
                if( projectile_component.get_owner() != obstacle.get_id() )
                {
                    geometry& obstacle_geom = obstacle.get_component_unsafe< geometry >();
                    if( obstacle_geom.intersects_with( projectile_geom ) )
                    {
                        if( obstacle.has_component< health >() )
                        {
                            health& obstacle_health = obstacle.get_component< health >();
                            obstacle_health.decrease( projectile_component.get_damage() );

                            if( !obstacle_health.alive() )
                            {
                                if( obstacle.has_component< tile_object >() )
                                {
                                    obstacle.remove_component< health >();
                                    obstacle.remove_component< non_traversible >();
                                    obstacle.get_component< tile_object >().set_tile_type( tile_type::empty );;
                                    obstacle.get_component< graphics >().set_image_path(
                                                tile_image_path( tile_type::empty ) );
                                }
                                else if( obstacle.has_component< tank_object >() )
                                {
                                    entities_removed_event.add_entity( obstacle.get_id() );
                                }
                            }
                        }

                        entities_removed_event.add_entity( projectile_entity.get_id() );

                        m_world.schedule_remove_entity( projectile_entity );
                        projectile_destroyed = true;
                    }
                }

                return !projectile_destroyed;
            } );
        }

        return true;
    } );

    if( !entities_removed_event.get_entities().empty() )
    {
        m_world.emit_event( entities_removed_event );
    }

    // Create new projectiles
    m_world.for_each< tank_object >( [ & ]( ecs::entity& tank_entity, tank_object& tank_info )
    {
        if( tank_info.has_fired() )
        {
            geometry& tank_geom = tank_entity.get_component< geometry >();
            movement& tank_move = tank_entity.get_component< movement >();

            movement_direction direction{ get_direction_by_rotation( tank_geom.get_rotation() ) };
            ecs::entity& proj_entity = create_entity_projectile( tank_geom.get_rect(),
                                                                 1,
                                                                 tank_move.get_speed() * 2, //TODO!
                                                                 direction,
                                                                 tank_entity.get_id(),
                                                                 m_world );

            event::projectile_fired event{ tank_entity,proj_entity  };
            m_world.emit_event( event );
            tank_info.set_fire_status( false );
        }

        return true;
    } );
}

}// system

}// game


