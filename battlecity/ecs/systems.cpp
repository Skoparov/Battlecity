#include "systems.h"

#include <random>
#include <cassert>
#include <algorithm>

#include "entity_factory.h"
#include "framework/details/rw_lock_guard.h"

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

    m_world.for_each_with< movement, geometry >( [ & ]( ecs::entity& curr_entity, movement& move, geometry& curr_geom )
    {
        ecs::rw_lock_guard< ecs::rw_lock > l{ move, ecs::lock_mode::write };

        if( move.get_move_direction() != movement_direction::none )
        {
            bool x_changed{ false };
            bool y_changed{ false };
            bool rotation_changed{ false };
            QRect rect_after_move;

            {
                ecs::rw_lock_guard< ecs::rw_lock > l{ curr_geom, ecs::lock_mode::write };

                int prev_rotation{ curr_geom.get_rotation() };
                bool is_flying{ curr_entity.has_component< flying >() };
                bool movement_valid{ true };

                rect_after_move = calc_move( move, curr_geom, *m_map_geom, is_flying );

                if( !is_flying )
                {
                    m_world.for_each_with< non_traversible, geometry >(
                    [ & ]( ecs::entity& other_entity, non_traversible&, geometry& other_geom )
                    {
                        if( curr_entity != other_entity )
                        {
                            ecs::rw_lock_guard< ecs::rw_lock > l{ other_geom, ecs::lock_mode::read };
                            if( other_geom.intersects_with( rect_after_move ) )
                            {
                                movement_valid = false;
                            }
                        }

                        return movement_valid;
                    } );
                }

                rotation_changed = prev_rotation != curr_geom.get_rotation();

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
            }

            if( x_changed || y_changed || rotation_changed )
            {
                event::geometry_changed event{ x_changed, y_changed, rotation_changed };
                event.set_cause_entity( curr_entity );
                m_world.emit_event( event );

                if( curr_entity.has_component< powerup_animations >() )
                {
                    powerup_animations& animations_comp = curr_entity.get_component< powerup_animations >();
                    for( auto& anim_pair : animations_comp.get_animations() )
                    {
                        if( anim_pair.first == powerup_type::shield )
                        {
                            ecs::entity& anim_entity = *anim_pair.second;

                            geometry& anim_geom = anim_entity.get_component< geometry >();
                            anim_geom.move_center_to( rect_after_move.center() );

                            event::geometry_changed anim_event{ x_changed, y_changed, false };
                            anim_event.set_cause_entity( anim_entity );
                            m_world.emit_event( anim_event );
                        }
                    }
                }
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

void projectile_system::kill_entity( ecs::entity& victim,
                                     const object_type& victim_type,
                                     ecs::entity* killer,
                                     const object_type& killer_type )
{
    using namespace component;

    bool count_kill{ false };

    victim.remove_component< non_traversible >();

    if( victim.has_component< graphics >() )
    {
        bool image_changed{ false };
        bool visibility_changed{ false };

        graphics& entity_graphics = victim.get_component< graphics >();

        if( victim_type == object_type::player_tank ||
            victim_type == object_type::enemy_tank ||
            victim_type == object_type::player_base )
        {
            entity_graphics.set_visible( false );
            visibility_changed = true;
            count_kill = true;
        }
        else if( victim_type == object_type::tile )
        {
            victim.remove_component< health >();
            victim.get_component< tile_object >().set_tile_type( tile_type::empty );
            entity_graphics.set_image_path( tile_image_path( tile_type::empty ) );
            image_changed = true;
        }

        event::graphics_changed graphics_changed_event{ image_changed, visibility_changed };
        graphics_changed_event.set_cause_entity( victim );
        m_world.emit_event( graphics_changed_event );
    }

    if( count_kill )
    {
        if( killer && killer->has_component< kills_counter >() )
        {
            killer->get_component< kills_counter >().increase( 1 );
        }

        event::entity_killed event{ victim_type, victim, killer_type, killer };
        m_world.emit_event( event );
    }
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

    uint32_t damage_to_do{ projectile_component.get_damage() };

    // Damage obstacle if it has health
    if( obstacle.has_component< shield >() )
    {
        shield& obstacle_shield = obstacle.get_component< shield >();
        uint32_t shield_power{ obstacle_shield.get_shield_health() };
        obstacle_shield.decrease( damage_to_do );

        damage_to_do = damage_to_do >= shield_power?
                    damage_to_do - shield_power : 0;

        if( !obstacle_shield.has_shield() )
        {
            obstacle.remove_component< shield >();
        }

        if( obstacle.has_component< powerup_animations >() )
        {
            powerup_animations& animations_comp =
                    obstacle.get_component< powerup_animations >();

            ecs::entity& anim_entity = animations_comp.get_animation( powerup_type::shield );
            anim_entity.get_component< animation_info >().force_stop();
            animations_comp.remove_animation( powerup_type::shield );
        }
    }

    if( damage_to_do && obstacle.has_component< health >() )
    {
        health& obstacle_health = obstacle.get_component< health >();
        ecs::rw_lock_guard< ecs::rw_lock > l{ obstacle_health, ecs::lock_mode::write };

        obstacle_health.decrease( projectile_component.get_damage() );
        if( !obstacle_health.alive() )
        {
            l.unlock();
            kill_entity( obstacle, obstacle_type, shooter, shooter_type );
        }
    }

    event::entity_hit event{ obstacle_type, obstacle, shooter_type, shooter };
    m_world.emit_event( event );
}

void projectile_system::handle_existing_projectiles()
{
    using namespace component;

    event::entities_removed entities_removed_event;

    m_world.for_each_with< projectile, geometry >(
    [ & ]( ecs::entity& projectile_entity, projectile& projectile_comp, geometry& projectile_geom )
    {
        if( !m_map_geom->intersects_with( projectile_geom ) )
        {
            projectile_comp.set_destroyed();
        }
        else
        {
            m_world.for_each_with< non_traversible, geometry >(
            [ & ]( ecs::entity& obstacle, non_traversible&, geometry& obstacle_geom )
            {
                if( projectile_comp.get_shooter_id() != obstacle.get_id() )
                {
                    if( obstacle_geom.intersects_with( projectile_geom ) )
                    {
                        handle_obstacle( obstacle, projectile_comp );

                        event::projectile_collision event_collision;
                        event_collision.set_cause_entity( obstacle );
                        m_world.emit_event( event_collision );

                        projectile_comp.set_destroyed();
                    }
                }

                return !projectile_comp.get_destroyed();
            } );
        }

        if( projectile_comp.get_destroyed() )
        {
            entities_removed_event.add_entity( object_type::projectile, projectile_entity );
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

    m_world.for_each_with< turret_object, geometry >(
    [ & ]( ecs::entity& turret_entity, turret_object& turret_info, geometry& tank_geom )
    {
        ecs::entity* proj_entity{ nullptr };

        {
            ecs::rw_lock_guard< ecs::rw_lock > l{ turret_info, ecs::lock_mode::write };

            if( turret_info.has_fired() )
            {
                QRect projectile_rect{ get_projectile_rect( tank_geom.get_rect(), m_projectile_size ) };

                movement_direction direction{ get_direction_by_rotation( tank_geom.get_rotation() ) };
                ecs::entity& entity = create_entity_projectile( projectile_rect,
                                                                m_damage,
                                                                m_speed,
                                                                direction,
                                                                turret_entity,
                                                                m_world );

                turret_info.set_fire_status( false );
                proj_entity = &entity;
            }
        }

        if( proj_entity )
        {
            event::projectile_fired event{ turret_entity, *proj_entity };
            m_world.emit_event( event );
        }

        return true;
    } );
}

//

respawn_system::respawn_system(ecs::world& world ) noexcept : ecs::system( world )
{
    m_world.subscribe< event::entity_killed >( *this );
    m_world.subscribe< event::powerup_taken >( *this );
}

respawn_system::~respawn_system()
{
    m_world.unsubscribe< event::entity_killed >( *this );
    m_world.unsubscribe< event::powerup_taken >( *this );
}

void respawn_system::init()
{
    std::list< ecs::entity* > tiles{
        m_world.get_entities_with_component< component::tile_object >() };

    for( const ecs::entity* e : tiles )
    {
        if( e->get_component< component::tile_object >().get_tile_type() == tile_type::empty )
        {
            m_empty_tiles.emplace_back( &e->get_component< component::geometry >() );
        }
    }

    auto enemies = m_world.get_entities_with_component< component::enemy >();
    for( ecs::entity* enemy : enemies )
    {
        m_death_info.emplace_back( death_info{ enemy, {} } );
    }

    auto curr_time = clock::now();
    auto power_ups = m_world.get_entities_with_component< component::power_up >();
    for( ecs::entity* enemy : power_ups )
    {
        m_death_info.emplace_back( death_info{ enemy, curr_time } );
    }
}

void respawn_system::tick()
{
    if( !m_empty_tiles.empty() && !m_death_info.empty()  )
    {
        std::vector< const component::geometry* > free_respawns{ get_free_respawns() };
        respawn_if_ready( m_death_info, free_respawns );
    }
}

void respawn_system::clean()
{
    m_empty_tiles.clear();
    m_death_info.clear();
}

void respawn_system::on_event( const event::entity_killed& event )
{
    maybe_add_to_respawn_list( event.get_subject() );
}

void respawn_system::on_event( const event::powerup_taken& event )
{
    maybe_add_to_respawn_list( event.get_subject() );
}

void respawn_system::maybe_add_to_respawn_list( ecs::entity& e )
{
    using namespace component;

    if( e.has_components< lifes, respawn_delay >() )
    {
        lifes& lifes_component = e.get_component< lifes >();
        ecs::rw_lock_guard< ecs::rw_lock > l{ lifes_component, ecs::lock_mode::write };

        if( lifes_component.has_life() )
        {
            m_death_info.emplace_back( death_info{ &e, clock::now() } );
            lifes_component.decrease( 1 );
        }
    }
}

void respawn_system::respawn_if_ready( std::list< death_info >& list,
                                       std::vector< const component::geometry* > free_respawns )
{
    using namespace component;
    using namespace std::chrono;

    auto curr_time = clock::now();

    for( auto it = list.begin(); it!=  list.end(); )
    {
        const death_info& info = *it;
        auto passed_since_death = duration_cast< milliseconds >( curr_time - info.death_time );
        auto resp_delay = info.entity->get_component< respawn_delay >().get_respawn_delay();

        if( passed_since_death >= resp_delay )
        {
            const geometry* respawn = free_respawns.back();
            free_respawns.pop_back();

            death_info& info = *it;
            respawn_entity( *info.entity, *respawn );
            list.erase( it++ );
        }
        else
        {
            ++it;
        }
    }
}

void respawn_system::respawn_entity( ecs::entity& entity, const component::geometry& respawn )
{
    using namespace component;

    {
        ecs::rw_lock_guard< ecs::rw_lock > l{ entity, ecs::lock_mode::write };

        if( entity.has_component< tank_object >() )
        {
            health& entity_health = entity.get_component< health >();
            entity_health.increase( entity_health.get_max_health() );
            entity.add_component< non_traversible >();
        }
        else if( entity.has_component< power_up >() )
        {
            entity.get_component< power_up >().
                    set_state( power_up::state::active );
        }

        entity.get_component< graphics >().set_visible( true );

        geometry& entity_geom = entity.get_component< geometry >();
        QRect entity_rect{ entity_geom.get_rect() };
        entity_rect.moveCenter( respawn.get_rect().center() );
        entity_geom.set_rect( entity_rect );
    }

    event::geometry_changed geometry_changed_event{ true, true, true };
    geometry_changed_event.set_cause_entity( entity );

    event::graphics_changed graphics_changed_event{ false, true };
    graphics_changed_event.set_cause_entity( entity );

    event::entity_respawned respawn_event;
    respawn_event.set_cause_entity( entity );

    m_world.emit_event( geometry_changed_event );
    m_world.emit_event( graphics_changed_event );
    m_world.emit_event( respawn_event );
}

std::vector< const component::geometry* > respawn_system::get_free_respawns()
{
    using namespace component;

    std::vector< const geometry* > free_respawns;

    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution< size_t >dist( 0, m_empty_tiles.size() - 1 );

    for( size_t num{ 0 }; num < m_death_info.size(); ++num )
    {

        bool respawn_free{ true };
        size_t attempt{ 0 };

        do
        {
            size_t respawn_index = dist( rng );
            const geometry* curr_geom{ m_empty_tiles[ respawn_index ] };

            m_world.for_each_with< non_traversible, geometry >(
            [ & ]( ecs::entity&, non_traversible&, geometry& obstacle )
            {
                respawn_free = !obstacle.intersects_with( *curr_geom );
                if( respawn_free )
                {
                    free_respawns.emplace_back( curr_geom );
                }

                ++attempt;

                return respawn_free;
            } );
        }
        while( !respawn_free && attempt < m_empty_tiles.size() );
    }

    std::shuffle( free_respawns.begin(), free_respawns.end(), rng );

    return free_respawns;
}

//

powerup_system::powerup_system( ecs::world& world ) noexcept : ecs::system( world ){}

void powerup_system::tick()
{
    using namespace component;

    m_world.for_each_with< power_up, geometry >(
    [ & ]( ecs::entity& powerup_entity, power_up& powerup_comp, geometry& powerup_geom )
    {
        if( powerup_comp.get_state() == power_up::state::active )
        {
            const powerup_type& type = powerup_comp.get_type();

            m_world.for_each_with< tank_object, geometry >(
            [&]( ecs::entity& tank, tank_object&, geometry& tank_geom )
            {
                bool need_to_continue{ true };

                if( tank_geom.intersects_with( powerup_geom ) )
                {
                    deactivate_powerup( powerup_entity, powerup_comp, tank );
                    apply_powerup( type, tank );
                    need_to_continue = false;
                }

                return need_to_continue;
            } );
        }

        return true;
    } );
}

void powerup_system::apply_powerup( const powerup_type& type, ecs::entity& target )
{
    if( type == powerup_type::shield )
    {
        target.add_component< component::shield >(
                    target.get_component< component::health >().get_max_health() );
    }
}

void powerup_system::deactivate_powerup( ecs::entity& powerup,
                                         component::power_up& comp,
                                         ecs::entity& taker )
{
    using namespace component;
    comp.set_state( power_up::state::waiting_to_respawn );

    {
        graphics& powerup_graphics = powerup.get_component< graphics >();
        ecs::rw_lock_guard< ecs::rw_lock > l{ powerup_graphics, ecs::lock_mode::write };

        powerup_graphics.set_visible( false );
    }

    event::graphics_changed graphics_changed_event{ false, true };
    graphics_changed_event.set_cause_entity( powerup );
    m_world.emit_event( graphics_changed_event );

    object_type tank_type{ taker.has_component< player >()?
                    object_type::player_tank : object_type::enemy_tank };

    event::powerup_taken powerup_taken_event{ object_type::power_up,
                                              powerup,
                                              tank_type,
                                              &taker };

    m_world.emit_event( powerup_taken_event );
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
    const health& player_base_health = m_player_base->get_component< health >();
    const kills_counter& player_kills = m_player->get_component< kills_counter >();
    const lifes& player_lifes = m_player->get_component< lifes >();

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

    if( event.get_performer_type() == object_type::player_tank )
    {
        const ecs::entity& victim = event.get_subject();
        if( victim.has_component< enemy >() )
        {
            uint32_t player_kills{ m_player->get_component< kills_counter >().get_kills() };
            ecs::entity* frag_entity{  m_frag_entities[ player_kills - 1 ] };

            component::graphics& graphics = frag_entity->get_component< component::graphics >();

            {
                ecs::rw_lock_guard< ecs::rw_lock > l{ graphics, ecs::lock_mode::write };
                graphics.set_visible( false );
            }

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

void tank_ai_system::init()
{
    m_enemies = m_world.get_entities_with_component< component::enemy >();

    auto players = m_world.get_entities_with_component< component::player >();
    if( players.size() != 1 )
    {
        throw std::logic_error{ "Exactly one player entity should exist" };
    }

    m_player = players.front();
}


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

    for( ecs::entity* enemy : m_enemies )
    {
        health& enemy_health = enemy->get_component< health >();
        ecs::rw_lock_guard< ecs::rw_lock > l{ enemy_health, ecs::lock_mode::read };

        if( enemy_health.alive() )
        {
            movement& move = enemy->get_component< movement >();
            ecs::rw_lock_guard< ecs::rw_lock > lm{ move, ecs::lock_mode::write };

            turret_object& enemy_turret = enemy->get_component< turret_object >();
            ecs::rw_lock_guard< ecs::rw_lock > let{ enemy_turret, ecs::lock_mode::write };

            if( move.get_move_direction() == movement_direction::none )
            {
                move.set_move_direction( generate_move_direction() );
            }

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
    m_player = nullptr;
}

animation_system::animation_system( ecs::world& world ) noexcept : ecs::system( world )
{
    m_world.subscribe< event::projectile_collision >( *this );
    m_world.subscribe< event::entity_respawned >( *this );
    m_world.subscribe< event::powerup_taken >( *this );
}

animation_system::~animation_system()
{
    m_world.unsubscribe< event::projectile_collision >( *this );
    m_world.unsubscribe< event::entity_respawned >( *this );
    m_world.unsubscribe< event::powerup_taken >( *this );
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
        const animation_start_info& anim_start_info = *iter;

        const component::animation_info& animation_comp =
                anim_start_info.entity->get_component_unsafe< component::animation_info >();

        if( !animation_comp.is_infinite() )
        {
            auto anum_duration = duration_cast< milliseconds >( now - anim_start_info.start );
            if( anum_duration >= animation_comp.get_duration() )
            {
                entities_removed_event.add_entity( object_type::animation, *anim_start_info.entity );

                event::animation_ended event{ animation_comp.get_type() };
                event.set_cause_entity( *anim_start_info.entity );
                m_world.emit_event( event );

                m_animations.erase( iter++ );
            }
            else
            {
                ++iter;
            }
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
    ecs::entity& entity = *event.get_cause_entity();
    const component::geometry& g = entity.get_component< component::geometry >();
    create_animation_entity( g.get_rect(), animation_type::explosion );
}

void mult_rect_size( QRect& rect, qreal mult )
{
    QPoint curr_center{ rect.center() };
    rect.setSize( rect.size() * mult );
    rect.moveCenter( curr_center );
}

void animation_system::on_event( const event::entity_respawned& event )
{
    ecs::entity& entity = *event.get_cause_entity();
    const component::geometry& g = entity.get_component< component::geometry >();
    QRect rect = g.get_rect();
    mult_rect_size( rect, 2 );

    create_animation_entity( rect, animation_type::respawn );
}

void animation_system::on_event( const event::powerup_taken& event )
{
    using namespace component;

    ecs::entity& entity = event.get_subject();
    const power_up& powerup_comp = entity.get_component< power_up >();
    const powerup_type& type = powerup_comp.get_type();

    if( powerup_comp.get_type() == powerup_type::shield )
    {
        ecs::entity& taker = *event.get_performer();

        powerup_animations& powerup_anim = taker.get_component< powerup_animations >();
        if( !powerup_anim.has_animation( type ) )
        {
            const component::geometry& g = taker.get_component< component::geometry >();
            QRect rect = g.get_rect();
            mult_rect_size( rect, 2 );

            ecs::entity& animation_entity =
                    create_animation_entity( rect, animation_type::shield );

            powerup_anim.add_animation( type, animation_entity );
        }
    }
}

ecs::entity& animation_system::create_animation_entity( const QRect& rect, const animation_type& type )
{
    const animation_data& data = m_animation_data.at( type );
    ecs::entity& e = create_animation( rect,
                                       data.frame_num,
                                       data.frame_rate,
                                       data.loops,
                                       data.duration,
                                       type,
                                       m_world );

    event::animation_started event_animation{ type };
    event_animation.set_cause_entity( e );

    animation_start_info info{ &e, clock::now() };
    m_animations.emplace_back( info );

    m_world.emit_event( event_animation );

    return e;
}

}// system

}// game


