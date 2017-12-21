#include "systems.h"

#include <random>
#include <cassert>
#include <algorithm>

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

void movement_system::init()
{
    auto map_entities = m_world.get_entities_with_component< component::game_map >();
    if( map_entities.size() != 1 )
    {
        throw std::logic_error{ "Exactly one map entity should exist" };
    }

    ecs::entity* map_entity{ map_entities.front() };
    m_map_geom = &map_entity->get_component_unsafe< component::geometry >();
}

QRect calc_move( component::movement& move,
                 component::geometry& obj_geom,
                 component::geometry& map_geom,
                 bool is_flying ) noexcept
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

    if( !is_flying )
    {
        bool on_border{ false };
        QRect map_rect{ map_geom.get_rect() };
        if( obj_rect.left() < map_rect.left() )
        {
            obj_rect.moveLeft( map_rect.left() );
            on_border = true;
        }
        else if( obj_rect.right() > map_rect.right() )
        {
            obj_rect.moveRight( map_rect.right() );
            on_border = true;
        }

        if( obj_rect.top() < map_rect.top() )
        {
            obj_rect.moveTop( map_rect.top() );
            on_border = true;
        }
        if( obj_rect.bottom() > map_rect.bottom() )
        {
            obj_rect.moveBottom( map_rect.bottom() );
            on_border = true;
        }

        if( on_border )
        {
            move.set_move_direction( movement_direction::none );
        }
    }

    return obj_rect;
}

void movement_system::tick()
{
    using namespace component;

    m_world.for_each< movement >( [ & ]( ecs::entity& curr_entity, movement& move )
    {
        if( move.get_move_direction() != movement_direction::none )
        {
            geometry& curr_geom = curr_entity.get_component_unsafe< geometry >();
            int prev_rotation{ curr_geom.get_rotation() };
            bool is_flying{ curr_entity.has_component< flying >() };
            bool movement_valid{ true };

            QRect rect_after_move{ calc_move( move, curr_geom, *m_map_geom, is_flying ) };

            if( !is_flying )
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
            else
            {
                move.set_move_direction( movement_direction::none );
            }

            if( x_changed || y_changed || rotation_changed )
            {
                event::geometry_changed event{ x_changed, y_changed, rotation_changed };
                event.set_cause_entity( curr_entity );
                m_world.emit_event( event );
            }
        }

        return true;
    } );
}

void movement_system::clean()
{
    m_map_geom = nullptr;
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

//

projectile_system::projectile_system( const QSize& projectile_size,
                                      uint32_t projectile_damage,
                                      uint32_t projectile_speed,
                                      ecs::world& world ) noexcept:
    ecs::system( world ),
    m_projectile_size( projectile_size ),
    m_damage( projectile_damage ),
    m_speed( projectile_speed ){}

void projectile_system::init()
{
    auto map_entities = m_world.get_entities_with_component< component::game_map >();
    if( map_entities.size() != 1 )
    {
        throw std::logic_error{ "Exactly one map entity should exist" };
    }

    ecs::entity* map_entity{ map_entities.front() };
    m_map_geom = &map_entity->get_component_unsafe< component::geometry >();
}

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

void projectile_system::clean()
{
    m_map_geom = nullptr;
}

void projectile_system::kill_entity( ecs::entity& entity )
{
    using namespace component;
    if( entity.has_component< graphics >() )
    {
        entity.get_component< graphics >().set_visible( false );
        event::graphics_changed graphics_changed_event{ false, true };
        graphics_changed_event.set_cause_entity( entity );
        m_world.emit_event( graphics_changed_event );
    }

    entity.remove_component< non_traversible >();
}

object_type get_object_type( const ecs::entity& entity )
{
    using namespace component;
    object_type result;

    if( entity.has_component< tile_object >() )
    {
        result = object_type::tile;
    }
    else if( entity.has_component< player >() )
    {
        result = object_type::player_tank;
    }
    else if( entity.has_component< enemy >() )
    {
        result = object_type::enemy_tank;
    }
    else if( entity.has_component< player_base >() )
    {
        result = object_type::player_base;
    }
    else
    {
        assert( false );
        throw std::logic_error{ "Unknown entity type" };
    }

    return result;
}

void projectile_system::handle_obstacle( ecs::entity& obstacle,
                                         const component::projectile& projectile_component )
{
    using namespace component;

    object_type obstacle_type{ get_object_type( obstacle ) };
    const object_type& shooter_type = projectile_component.get_shooter_type();

    ecs::entity* shooter{ nullptr };
    ecs::entity_id shooter_id{ projectile_component.get_shooter_id() };
    if( m_world.entity_present( shooter_id ) )
    {
        shooter = &m_world.get_entity( shooter_id );
    }

    // Damage obstacle if it has health
    if( obstacle.has_component< health >() )
    {
        health& obstacle_health = obstacle.get_component< health >();
        obstacle_health.decrease( projectile_component.get_damage() );

        if( !obstacle_health.alive() )
        {
            if( obstacle_type == object_type::tile )
            {
                obstacle.remove_component< health >();
                obstacle.remove_component< non_traversible >();
                obstacle.get_component< tile_object >().set_tile_type( tile_type::empty );
                obstacle.get_component< graphics >().set_image_path( tile_image_path( tile_type::empty ) );
            }
            else if( obstacle_type == object_type::player_tank ||
                     obstacle_type == object_type::enemy_tank ||
                     obstacle_type == object_type::player_base )
            {
                kill_entity( obstacle );

                if( shooter && shooter->has_component< kills_counter >() )
                {
                    shooter->get_component< kills_counter >().increase( 1 );
                }

                event::entity_killed event{ obstacle_type, obstacle, shooter_type, shooter };
                m_world.emit_event( event );
            }
        }
    }

    event::entity_hit event{ obstacle_type, obstacle, shooter_type, shooter };
    m_world.emit_event( event );
}

void projectile_system::handle_existing_projectiles()
{
    using namespace component;

    // Calc existing projectiles
    event::entities_removed entities_removed_event;

    m_world.for_each< projectile >( [ & ]( ecs::entity& projectile_entity, projectile& projectile_component )
    {
        if( !projectile_component.get_destroyed() )
        {
            geometry& projectile_geom = projectile_entity.get_component_unsafe< geometry >();

            if( !m_map_geom->intersects_with( projectile_geom ) )
            {
                projectile_component.set_destroyed();
            }
            else
            {
                m_world.for_each< non_traversible >( [ & ]( ecs::entity& obstacle, non_traversible& )
                {
                    if( projectile_component.get_shooter_id() != obstacle.get_id() )
                    {
                        geometry& obstacle_geom = obstacle.get_component_unsafe< geometry >();
                        if( obstacle_geom.intersects_with( projectile_geom ) )
                        {
                            handle_obstacle( obstacle, projectile_component );

                            event::projectile_collision event_collision;
                            event_collision.set_cause_entity( obstacle );
                            m_world.emit_event( event_collision );

                            projectile_component.set_destroyed();
                        }
                    }

                    return !projectile_component.get_destroyed();;
                } );
            }

            if( projectile_component.get_destroyed() )
            {
                entities_removed_event.add_entity( object_type::projectile, projectile_entity );
            }
        }

        return true;
    } );

    if( !entities_removed_event.empty() )
    {
        m_world.emit_event( entities_removed_event );
    }
}

void projectile_system::create_new_projectiles()
{
    using namespace component;

    m_world.for_each< turret_object >( [ & ]( ecs::entity& turret_entity, turret_object& turret_info )
    {
        if( turret_info.has_fired() )
        {
            geometry& tank_geom = turret_entity.get_component< geometry >();
            QRect projectile_rect{ get_projectile_rect( tank_geom.get_rect(), m_projectile_size ) };

            movement_direction direction{ get_direction_by_rotation( tank_geom.get_rotation() ) };
            ecs::entity& proj_entity = create_entity_projectile( projectile_rect,
                                                                 m_damage,
                                                                 m_speed,
                                                                 direction,
                                                                 turret_entity,
                                                                 m_world );

            event::projectile_fired event{ turret_entity, proj_entity };
            m_world.emit_event( event );
            turret_info.set_fire_status( false );
        }

        return true;
    } );
}

//

respawn_system::~respawn_system()
{
    m_world.unsubscribe< event::entity_killed >( *this );
}

void respawn_system::respawn_entity( ecs::entity& entity, const component::geometry& respawn )
{
    using namespace component;
    entity.get_component< movement >().set_move_direction( movement_direction::none );

    health& entity_health = entity.get_component< health >();
    entity_health.increase( entity_health.get_max_health() );

    entity.add_component< non_traversible >();

    entity.get_component< graphics >().set_visible( true );
    entity.get_component< geometry >().set_rect( respawn.get_rect() );

    event::geometry_changed geometry_changed_event{ true, true, true };
    geometry_changed_event.set_cause_entity( entity );

    event::graphics_changed graphics_changed_event{ false, true };
    graphics_changed_event.set_cause_entity( entity );

    m_world.emit_event( geometry_changed_event );
    m_world.emit_event( graphics_changed_event );
}

void respawn_system::respawn_list( std::list< death_info >& list,
                                   std::vector< const component::geometry* > free_respawns )
{
    using namespace component;
    using namespace std::chrono;

    auto curr_time = clock::now();

    while( !free_respawns.empty() )
    {
        auto it = std::find_if( list.begin(), list.end(),[ & ]( const death_info& info )
        {
            return ( duration_cast< milliseconds >( curr_time - info.death_time ) >= m_respawn_delay );
        } );

        if( it != list.end() )
        {
            const geometry* respawn = free_respawns.back();
            free_respawns.pop_back();

            death_info& info = *it;
            respawn_entity( *info.entity, *respawn );
            list.erase( it );
        }
        else
        {
            break;
        }
    }
}

void respawn_system::tick()
{
    if( !m_respawn_points.empty() )
    {
        std::vector< const component::geometry* > free_respawns{ get_free_respawns() };

        if( !m_players_death_info.empty() )
        {
            respawn_list( m_players_death_info, free_respawns );
        }

        if( !m_enemies_death_info.empty() )
        {
            respawn_list( m_enemies_death_info, free_respawns );
        }
    }
}

void respawn_system::init()
{
    std::list< ecs::entity* > respawn_point_entities{
        m_world.get_entities_with_component< component::respawn_point >() };

    for( const ecs::entity* e : respawn_point_entities )
    {
        m_respawn_points.emplace_back( &e->get_component< component::geometry >() );
    }

    auto enemies = m_world.get_entities_with_component< component::enemy >();
    for( ecs::entity* enemy : enemies )
    {
        m_enemies_death_info.emplace_back( death_info{ enemy, {} } );
    }
}

void respawn_system::clean()
{
    m_respawn_points.clear();
    m_players_death_info.clear();
    m_enemies_death_info.clear();
}

void respawn_system::on_event( const event::entity_killed& event )
{
    using namespace component;

    auto curr_time = clock::now();
    ecs::entity& victim = event.get_victim();

    if( victim.has_component< lifes >() )
    {
        lifes& victim_lifes_component = victim.get_component< lifes >();
        if( victim_lifes_component.has_life() )
        {
            if( event.get_victim().has_component< component::player >() )
            {
                m_players_death_info.emplace_back( death_info{ &event.get_victim(), curr_time } );
            }
            else if( event.get_victim().has_component< component::enemy >() )
            {
                m_enemies_death_info.emplace_back( death_info{ &event.get_victim(), curr_time } );
            }

            victim_lifes_component.decrease( 1 );
        }
    }
}

std::vector< const component::geometry* > respawn_system::get_free_respawns()
{
    using namespace component;

    size_t free_respawns_needed{ m_players_death_info.size() +
                m_enemies_death_info.size() };

    std::vector< const geometry* > free_respawns;

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

    static std::default_random_engine engine{};
    std::shuffle( free_respawns.begin(), free_respawns.end(), engine );

    return free_respawns;
}

//

win_defeat_system::win_defeat_system( uint32_t kills_to_win, ecs::world& world ) noexcept:
    ecs::system( world ),
    m_kills_to_win( kills_to_win )
{
    m_world.subscribe< event::entity_killed >( *this );
}

win_defeat_system::~win_defeat_system()
{
    m_world.unsubscribe< event::entity_killed >( *this );
}

void win_defeat_system::init()
{
    auto entities = m_world.get_entities_with_component< component::frag >();
    std::copy( entities.begin(), entities.end(), std::back_inserter( m_frag_entities ) );
    std::sort( m_frag_entities.begin(), m_frag_entities.end(),
               []( const ecs::entity* l, const ecs::entity* r )
    {
        return l->get_component< component::frag >().get_num() >
               r->get_component< component::frag >().get_num();
    } );

    auto players = m_world.get_entities_with_component< component::player >();
    if( players.size() != 1 )
    {
        throw std::logic_error{ "Exactly one player entity should exist" };
    }

    m_player = players.front();

    auto player_bases = m_world.get_entities_with_component< component::player_base >();
    if( player_bases.size() != 1 )
    {
        throw std::logic_error{ "Exactly one player base entity should exist" };
    }

    m_player_base = player_bases.front();
}

void win_defeat_system::tick()
{
    using namespace component;
    health& player_base_health = m_player_base->get_component< health >();
    kills_counter& player_kills = m_player->get_component< kills_counter >();
    lifes& player_lifes = m_player->get_component< lifes >();

    if( !player_base_health.alive() || !player_lifes.has_life() )
    {
        m_world.emit_event( event::level_completed{ level_game_result::defeat } );
    }
    else if( player_kills.get_kills() == m_kills_to_win )
    {
        m_world.emit_event( event::level_completed{ level_game_result::victory } );
    }
}

void win_defeat_system::clean()
{
    m_player = m_player_base = nullptr;
    m_frag_entities.clear();
}

void win_defeat_system::on_event(const event::entity_killed& event )
{
    using namespace component;

    if( event.get_shooter_type() == object_type::player_tank )
    {
        const ecs::entity& victim = event.get_victim();
        if( victim.has_component< enemy >() )
        {
            uint32_t player_kills{ m_player->get_component< kills_counter >().get_kills() };
            ecs::entity* frag_entity{  m_frag_entities[ player_kills - 1 ] };

            frag_entity->get_component< component::graphics >().set_visible( false );
            event::graphics_changed event_graphics{ false, true };
            event_graphics.set_cause_entity( *frag_entity );
            m_world.emit_event( event_graphics );
        }
    }
}

//

tank_ai_system::tank_ai_system( float chance_to_fire, ecs::world& world ) noexcept :
    ecs::system( world ),
    m_chance_to_fire( chance_to_fire ){}


movement_direction generate_move_direction()
{
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_int_distribution< std::mt19937::result_type > dist{ 0, 3 };
    return static_cast< movement_direction >( dist( rng ) );
}

bool tank_ai_system::maybe_fire()
{
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_int_distribution< std::mt19937::result_type >dist{ 0, 100 };
    return ( dist( rng ) < m_chance_to_fire * 100 );
}

void tank_ai_system::tick()
{
    using namespace component;
    if( m_enemies.empty() )
    {
        m_enemies = m_world.get_entities_with_component< enemy >();
    }

    for( ecs::entity* enemy : m_enemies )
    {
        if( enemy->get_component< health >().alive() )
        {
            movement& move = enemy->get_component< movement >();
            if( move.get_move_direction() == movement_direction::none )
            {
                move.set_move_direction( generate_move_direction() );
            }

            turret_object& enemy_turret = enemy->get_component< turret_object >();
            if( !enemy_turret.has_fired() && maybe_fire() )
            {
                enemy_turret.set_fire_status( true );
            }
        }
    }
}

void tank_ai_system::clean()
{
    m_enemies.clear();
}

animation_system::animation_system( ecs::world& world ) noexcept : ecs::system( world )
{
    m_world.subscribe< event::projectile_collision >( *this );
}

animation_system::~animation_system()
{
    m_world.unsubscribe< event::projectile_collision >( *this );
}

void animation_system::clean()
{
    m_animations.clear();
}

void animation_system::tick()
{
    using namespace std::chrono;

    event::entities_removed entities_removed_event;

    auto now = clock::now();
    for( auto iter = m_animations.begin(); iter != m_animations.end(); )
    {
        const animation_info& anim = *iter;

        const component::animation_info& info =
                anim.entity->get_component_unsafe< component::animation_info >();

        auto anum_duration = duration_cast< milliseconds >( now - anim.start );
        if( anum_duration >= info.get_duration() )
        {
            entities_removed_event.add_entity( object_type::animation, *anim.entity );

            event::animation_ended event{ info.get_type() };
            event.set_cause_entity( *anim.entity );
            m_world.emit_event( event );

            m_animations.erase( iter++ );
        }
        else
        {
            ++iter;
        }
    }

    if( !entities_removed_event.empty() )
    {
        m_world.emit_event( entities_removed_event );
    }
}

void animation_system::add_animation_settings( const animation_type& type,
                                               const animation_data& data )
{
    m_animation_data[ type ] = data;
}

void animation_system::on_event( const event::projectile_collision& event )
{
    const component::geometry& g = event.get_cause_entity()->
            get_component< component::geometry >();

    const animation_data& data = m_animation_data.at( animation_type::explosion );
    ecs::entity& e = create_explosion( g.get_rect(),
                                       data.frame_num,
                                       data.frame_rate,
                                       data.loops,
                                       data.duration,
                                       m_world );

    event::animation_started event_explosion{ animation_type::explosion };
    event_explosion.set_cause_entity( e );

    animation_info info{ &e, clock::now() };
    m_animations.emplace_back( info );

    m_world.emit_event( event_explosion );
}

}// system

}// game


