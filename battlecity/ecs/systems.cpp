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

projectile_system::projectile_system( const QSize& projectile_size,
                                      uint32_t projectile_damage,
                                      uint32_t projectile_speed,
                                      ecs::world& world ) noexcept:
    ecs::system( world ),
    m_projectile_size( projectile_size ),
    m_damage( projectile_damage ),
    m_speed( projectile_speed ){}

QRect get_projectile_rect( const QRect& tank_rect, const QSize& proj_size )
{
    QPoint projectile_top_left{ tank_rect.center() };
    projectile_top_left.setX( projectile_top_left.x() - proj_size.width() / 2 );
    projectile_top_left.setY( projectile_top_left.y() - proj_size.height() / 2 );
    return QRect{ projectile_top_left, proj_size };
}

void projectile_system::tick()
{
    handle_existing_projectiles();
    create_new_projectiles();
}

void projectile_system::kill_entity( ecs::entity& entity )
{
    using namespace component;
    if( entity.has_component< graphics >() )
    {
        entity.get_component< graphics >().set_visible( false );
        event::graphics_changed graphics_changed_event{ false, true };
        graphics_changed_event.add_entity( entity.get_id() );
        m_world.emit_event( graphics_changed_event );
    }

    entity.remove_component< non_traversible >();
}

void projectile_system::handle_obstacle(ecs::entity& obstacle,
                                        const component::projectile& projectile_component )
{
    using namespace component;

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
                obstacle.get_component< tile_object >().set_tile_type( tile_type::empty );
                obstacle.get_component< graphics >().set_image_path(
                            tile_image_path( tile_type::empty ) );
            }
            if( obstacle.has_component< player >() )
            {
                kill_entity( obstacle );
                m_world.emit_event( event::player_killed{} );
            }
            else if( obstacle.has_component< enemy >() )
            {
                kill_entity( obstacle );
                m_world.emit_event( event::enemy_killed{} );
            }
            else if( obstacle.has_component< player_base >() )
            {
                m_world.emit_event( event::player_base_killed{} );
            }
        }
    }
}

void projectile_system::handle_existing_projectiles()
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
                        handle_obstacle( obstacle, projectile_component );
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
}

void projectile_system::create_new_projectiles()
{
    using namespace component;

    // Create new projectiles
    m_world.for_each< tank_object >( [ & ]( ecs::entity& tank_entity, tank_object& tank_info )
    {
        if( tank_info.has_fired() )
        {
            geometry& tank_geom = tank_entity.get_component< geometry >();
            QRect projectile_rect{ get_projectile_rect( tank_geom.get_rect(), m_projectile_size ) };

            movement_direction direction{ get_direction_by_rotation( tank_geom.get_rotation() ) };
            ecs::entity& proj_entity = create_entity_projectile( projectile_rect,
                                                                 m_damage,
                                                                 m_speed,
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

//

respawn_system::respawn_system( uint32_t enemies_to_respawn, ecs::world& world ) noexcept:
    ecs::system( world ),
    m_enemies_to_respawn( enemies_to_respawn ),
    m_max_enemies( enemies_to_respawn )
{
    m_world.subscribe< event::enemy_killed >( *this );
    m_world.subscribe< event::player_killed >( *this );
}

respawn_system::~respawn_system()
{
    m_world.unsubscribe< event::enemy_killed >( *this );
    m_world.unsubscribe< event::player_killed >( *this );
}

void respawn_system::tick()
{
    using namespace component;

    if( m_player_needs_respawn || m_enemies_to_respawn )
    {
        if( m_respawn_points.empty() )
        {
            std::list< ecs::entity* > respawn_point_entities{
                m_world.get_entities_with_component< respawn_point >() };

            for( const ecs::entity* e : respawn_point_entities )
            {
                m_respawn_points.emplace_back( &e->get_component< geometry >() );
            }
        }

        if( !m_respawn_points.empty() )
        {

            std::vector< const geometry* > free_respawns{ get_free_respawns() };
            uint32_t curr_respawn_idx{ 0 };
            if( m_player_needs_respawn && !free_respawns.empty() )
            {
                m_player_needs_respawn = false;
               // ecs::entity* player{ m_world.get_entities_with_component< tank_object >().front() };
                ++curr_respawn_idx;
            }

            if( m_enemies_to_respawn && curr_respawn_idx < free_respawns.size() )
            {
                std::list< ecs::entity* > enemies{ m_world.get_entities_with_component< enemy >() };
                enemies.erase( std::remove_if( enemies.begin(), enemies.end(),
                                               []( ecs::entity* e )
                {
                    return e->get_component< health >().alive();
                } ), enemies.end() );

                for( ecs::entity* enemy : enemies )
                {
                    if( curr_respawn_idx == free_respawns.size() )
                    {
                        break;
                    }

                    health& enemy_health = enemy->get_component< health >();
                    enemy_health.increase( enemy_health.get_max_health() );

                    enemy->add_component< non_traversible >();

                    enemy->get_component< graphics >().set_visible( true );

                    const geometry* respawn{ free_respawns[ curr_respawn_idx ] };
                    enemy->get_component< geometry >().set_rect( respawn->get_rect() );

                    event::geometry_changed geometry_changed_event{ true, true, true };
                    geometry_changed_event.add_entity( enemy->get_id() );

                    event::graphics_changed graphics_changed_event{ false, true };
                    graphics_changed_event.add_entity( enemy->get_id() );

                    m_world.emit_event( geometry_changed_event );
                    m_world.emit_event( graphics_changed_event );

                    ++curr_respawn_idx;
                    --m_enemies_to_respawn;
                }
            }
        }
    }
}

void respawn_system::clean()
{
    m_respawn_points.clear();
    m_player_needs_respawn = false;
    m_enemies_to_respawn = m_max_enemies;
}

void respawn_system::on_event( const event::player_killed& )
{
    m_player_needs_respawn = true;
}

void respawn_system::on_event( const event::enemy_killed& )
{
    ++m_enemies_to_respawn;
}

std::vector< const component::geometry* > respawn_system::get_free_respawns()
{
    using namespace component;
    std::vector< const geometry* > free_respawns;

    uint32_t free_respawns_needed{ m_enemies_to_respawn };
    if( m_player_needs_respawn )
    {
        ++free_respawns_needed;
    }

    for( const geometry* curr_geom : m_respawn_points )
    {
        bool respawn_free{ true };

        m_world.for_each< tank_object >( [ & ]( ecs::entity& e, tank_object& )
        {
            if( e.has_component< non_traversible >() &&
                e.get_component< geometry >().intersects_with( *curr_geom ) )
            {
                respawn_free = false;
            }

            return respawn_free;
        } );

        if( respawn_free )
        {
            free_respawns.emplace_back( curr_geom );
        }

        if( free_respawns.size() == free_respawns_needed )
        {
            break;
        }
    }

    return free_respawns;
}

win_defeat_system::win_defeat_system( uint32_t kills_to_win,
                                      uint32_t player_lifes,
                                      ecs::world& world ) noexcept:
    ecs::system( world ),
    m_kills_to_win( kills_to_win ),
    m_player_lifes( player_lifes ),
    m_player_lifes_left( player_lifes )
{
    m_world.subscribe< event::enemy_killed >( *this );
    m_world.subscribe< event::player_killed >( *this );
    m_world.subscribe< event::player_base_killed >( *this );
}

win_defeat_system::~win_defeat_system()
{
    m_world.unsubscribe< event::enemy_killed >( *this );
    m_world.unsubscribe< event::player_killed >( *this );
    m_world.unsubscribe< event::player_base_killed >( *this );
}

void win_defeat_system::tick()
{
    if( m_player_base_killed || !m_player_lifes_left )
    {
        m_world.emit_event( event::level_completed{ level_game_result::defeat } );
    }
    else if( m_player_kills == m_kills_to_win )
    {
        m_world.emit_event( event::level_completed{ level_game_result::victory } );
    }
}

void win_defeat_system::clean()
{
    m_player_kills = 0;
    m_player_base_killed = false;
    m_player_lifes_left = m_player_lifes;
}

void win_defeat_system::on_event( const event::enemy_killed& )
{
    ++m_player_kills;
}

void win_defeat_system::on_event( const event::player_killed& )
{
    if( m_player_lifes_left )
    {
        --m_player_lifes_left;
    }
}

void win_defeat_system::on_event( const event::player_base_killed& )
{
    m_player_base_killed = true;
}

}// system

}// game


