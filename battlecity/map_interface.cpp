#include "map_interface.h"

#include <unordered_set>

#include "controller.h"

static const uint32_t map_switch_pause_duration{ 3000 };

namespace game
{

qml_map_interface::qml_map_interface( controller& controller,
                                      QObject* parent ) :
    QObject( parent ),
    m_controller( controller )
{
    m_controller.subscribe< event::projectile_fired >( *this );
    m_controller.subscribe< event::entities_removed >( *this );
    m_controller.subscribe< event::entity_killed >( *this );
    m_controller.subscribe< event::explosion_started >( *this );
    m_controller.subscribe< event::entity_hit >( *this );

    m_hide_announcement_timer = new QTimer{ this };
    connect( m_hide_announcement_timer, SIGNAL( timeout() ), this, SLOT( hide_announcement() ) );

    connect( this, SIGNAL(projectile_fired(ecs::entity*) ), SLOT( projectile_fired_slot(ecs::entity*) )  );
    connect( this, SIGNAL(explosion_started(ecs::entity*) ), SLOT( explosion_started_slot(ecs::entity*) )  );
    connect( this, SIGNAL(objects_need_to_be_removed() ), SLOT( remove_dead_objects() )  );
}

qml_map_interface::~qml_map_interface()
{
    m_controller.unsubscribe< event::projectile_fired >( *this );
    m_controller.unsubscribe< event::entities_removed >( *this );
    m_controller.unsubscribe< event::entity_killed >( *this );
    m_controller.unsubscribe< event::explosion_started >( *this );
    m_controller.unsubscribe< event::entity_hit >( *this );
}

void qml_map_interface::add_object( const object_type& type, ecs::entity& entity )
{
    std::unique_ptr< base_map_object > map_object;

    switch( type )
    {
    case object_type::tile:
        map_object.reset( new graphics_map_object{ &entity, type } );
        m_tiles.append( dynamic_cast< graphics_map_object* >( map_object.get() ) );
        break;
    case object_type::player_tank:
        map_object.reset( new tank_map_object{ &entity, type } );
        m_player_tanks.append( dynamic_cast< tank_map_object* >( map_object.get() ) );
        break;
    case object_type::enemy_tank:
        map_object.reset( new tank_map_object{ &entity, type } );
        m_enemy_tanks.append( dynamic_cast< tank_map_object* >( map_object.get() ) );
        break;
    case object_type::player_base:
        map_object.reset( new graphics_map_object{ &entity, type } );
        m_player_bases.append( dynamic_cast< graphics_map_object* >( map_object.get() ) );
        break;
    case object_type::projectile:
        map_object.reset( new movable_map_object{ &entity, type } );
        m_projectiles.append( dynamic_cast< movable_map_object* >( map_object.get() ) );
        break;
    case object_type::frag:
        map_object.reset( new graphics_map_object{ &entity, type } );
        m_remaining_frags.append( dynamic_cast< graphics_map_object* >( map_object.get() ) );
        break;
    case object_type::explosion:
        map_object.reset( new graphics_map_object{ &entity, type } );
        m_explosions.append( dynamic_cast< graphics_map_object* >( map_object.get() ) );
        break;
    case object_type::respawn_point: break;
    default:
        assert( false );
    }

    m_map_objects[ type ].emplace( entity.get_id(), std::move( map_object ) );
}

void qml_map_interface::remove_all()
{
    m_tiles.clear();
    m_player_tanks.clear();
    m_enemy_tanks.clear();
    m_player_bases.clear();
    m_projectiles.clear();
    m_remaining_frags.clear();
    m_projectiles.clear();
    m_explosions.clear();

    update_all();

    m_map_objects.clear();
}

void qml_map_interface::level_started( const QString& level )
{
    update_announcement( QString{ "Level %1" }.arg( level ), false );
    m_hide_announcement_timer->start( map_switch_pause_duration );

    update_all();
}

void qml_map_interface::level_ended( const level_game_result& result )
{
    update_announcement( QString{ "%1" }.arg( result == level_game_result::victory?
                                                  "Victory" : "Defeat" ),
                         true );
}

void qml_map_interface::game_ended( const level_game_result& result )
{

    update_announcement( QString{ "Game %1" }.arg( result == level_game_result::victory?
                                                       "completed" : "lost" ),
                         true );
}

int qml_map_interface::get_rows_num() const noexcept
{
    return m_controller.get_rows_num();
}

int qml_map_interface::get_columns_num() const noexcept
{
    return m_controller.get_columns_num();
}

int qml_map_interface::get_tile_width() const noexcept
{
    return m_controller.get_tile_width();
}

int qml_map_interface::get_tile_height() const noexcept
{
    return m_controller.get_tile_height();
}

int qml_map_interface::get_frag_width() const noexcept
{
    return !m_remaining_frags.empty()?
                m_remaining_frags[ 0 ]->get_width() : 0;
}

QString qml_map_interface::get_announcement_text() const
{
    return m_announcement;
}

bool qml_map_interface::get_announecement_visible() const noexcept
{
    return m_announcement_visible;
}

int qml_map_interface::get_remaining_frags_num() const noexcept
{
    return m_remaining_frags.size();
}

int qml_map_interface::get_player_remaining_lifes() const noexcept
{
    return m_controller.get_player_remaining_lifes();
}

int qml_map_interface::get_base_remaining_health() const noexcept
{
    return m_controller.get_base_remaining_health();
}

QQmlListProperty< graphics_map_object > qml_map_interface::get_tiles()
{
    return QQmlListProperty< graphics_map_object >{ this, m_tiles };
}

QQmlListProperty< graphics_map_object > qml_map_interface::get_player_bases()
{
    return QQmlListProperty< graphics_map_object >{ this, m_player_bases };
}

QQmlListProperty< tank_map_object > qml_map_interface::get_player_tanks()
{
    return QQmlListProperty< tank_map_object >{ this, m_player_tanks };
}

QQmlListProperty< tank_map_object > qml_map_interface::get_enemy_tanks()
{
    return QQmlListProperty< tank_map_object >{ this, m_enemy_tanks };
}

QQmlListProperty<movable_map_object> qml_map_interface::get_projectiles()
{
    return QQmlListProperty< movable_map_object >{ this, m_projectiles };
}

QQmlListProperty<graphics_map_object> qml_map_interface::get_remaining_frags()
{
    return QQmlListProperty< graphics_map_object >{ this, m_remaining_frags };
}

QQmlListProperty< graphics_map_object > qml_map_interface::get_explosions()
{
    return QQmlListProperty< graphics_map_object >{ this, m_explosions };
}

void qml_map_interface::on_event( const event::projectile_fired& event )
{
    emit projectile_fired( &event.get_projectile() );
}

void qml_map_interface::projectile_fired_slot( ecs::entity* e)
{
    add_object( object_type::projectile, *e );
    objects_of_type_changed( object_type::projectile );
}

void qml_map_interface::explosion_started_slot( ecs::entity* e )
{
    add_object( object_type::explosion, *e );
    objects_of_type_changed( object_type::explosion );
}

void qml_map_interface::remove_dead_objects()
{
    for( const auto& pair_data : objects_to_remove )
    {
        const object_type& type = pair_data.first;
        for( const auto& obj : pair_data.second )
        {
            remove_object_from_model( type, obj.get() );
        }

        objects_of_type_changed( type );
    }

    objects_to_remove.clear();
}

void qml_map_interface::on_event( const event::entity_killed& event )
{
    const object_type& victim_type = event.get_victim_type();
    if( victim_type ==  object_type::player_tank )
    {
        emit player_remaining_lifes_changed( m_controller.get_player_remaining_lifes() );
    }
    else if( victim_type == object_type::enemy_tank )
    {
        objects_of_type_changed( object_type::frag );
    }
}

void qml_map_interface::on_event( const event::entity_hit& event )
{
    const object_type& victim_type = event.get_victim_type();
    if( victim_type ==  object_type::player_base )
    {
        emit base_remaining_health_changed( m_controller.get_base_remaining_health() );
    }
}

void qml_map_interface::on_event( const event::entities_removed& event )
{
    const auto& entities_to_remove = event.get_removed_entities();

    for( const auto& type_to_remove_and_entities : entities_to_remove )
    {
        const object_type& type = type_to_remove_and_entities.first;

        auto object_map_it = m_map_objects.find( type );
        if( object_map_it != m_map_objects.end() )
        {
            object_umap& objects_map = object_map_it->second;
            const std::list< ecs::entity* >& entities_of_type_to_remove =
                    type_to_remove_and_entities.second;

            for( const ecs::entity* entity_to_remove : entities_of_type_to_remove )
            {
                auto object_it = objects_map.find( entity_to_remove->get_id() );
                if( object_it != objects_map.end() )
                {
                    objects_to_remove[ type ].emplace_back( std::move( object_it->second ) );
                    objects_map.erase( object_it );
                }
            }

            if( objects_map.empty() )
            {
                m_map_objects.erase( object_map_it );
            }
        }
    }

    emit objects_need_to_be_removed();
}

void qml_map_interface::on_event( const event::explosion_started& event )
{
    emit explosion_started( event.get_cause_entity() );
}

void qml_map_interface::explosion_ended( unsigned int id )
{
    auto explosions_it = m_map_objects.find( object_type::explosion );
    if( explosions_it != m_map_objects.end() )
    {
        std::unique_ptr< base_map_object > explosion;

        object_umap& explosions = explosions_it->second;
        auto explosion_it = explosions.find( id );

        if( explosion_it != explosions.end() )
        {
            explosion = std::move( explosion_it->second );
            explosions.erase( explosion_it );

            remove_object_from_model( object_type::explosion, explosion.get() );
            objects_of_type_changed( object_type::explosion );

            event::explosion_ended event;
            event.set_cause_id( id );
            m_controller.emit_event( event );
        }
    }
}

void qml_map_interface::pause_resume()
{
    const controller_state& state = m_controller.get_state();
    if( state == controller_state::paused )
    {
        m_controller.resume();
    }
    else if( state == controller_state::running )
    {
        m_controller.pause();
    }
}

void qml_map_interface::hide_announcement()
{
    m_hide_announcement_timer->stop();
    m_announcement_visible = false;
    emit announcement_visibility_changed( m_announcement_visible );
}

void qml_map_interface::update_announcement( const QString& text, bool send_update )
{
    m_announcement = text;
    m_announcement_visible = true;

    if( send_update )
    {
        emit announcement_changed( m_announcement );
        emit announcement_visibility_changed( m_announcement_visible );
    }
}

void qml_map_interface::objects_of_type_changed( const object_type& type )
{
    switch( type )
    {
    case object_type::tile:
        emit tiles_changed( get_tiles() );
        break;
    case object_type::player_tank:
        emit player_tanks_changed( get_player_tanks() );
        break;
    case object_type::enemy_tank:
        emit enemy_tanks_changed( get_enemy_tanks() );
        break;
    case object_type::player_base:
        emit player_bases_changed( get_player_bases() );
        break;
    case object_type::projectile:
        emit projectiles_changed( get_projectiles() );
        emit tiles_changed( get_tiles() );
        break;
    case object_type::frag:
        emit remaining_frags_changed( get_remaining_frags() );
        break;
    case object_type::explosion:
        emit explosions_changed( get_explosions() );
        break;
    default:
        assert( false );
    }
}

void qml_map_interface::remove_object_from_model( const object_type& type, base_map_object* obj )
{
    switch( type )
    {
    case object_type::projectile:
        m_projectiles.erase( std::remove( m_projectiles.begin(), m_projectiles.end(), obj ),
                             m_projectiles.end() );
        break;
    case object_type::explosion:
    {
        m_explosions.erase( std::remove( m_explosions.begin(), m_explosions.end(), obj ),
                             m_explosions.end() );
        break;
    }
    default:
        assert( false );
    }
}

void qml_map_interface::update_all()
{
    emit rows_num_changed( get_rows_num() );
    emit columns_num_changed( get_columns_num() );
    emit tiles_changed( get_tiles() );
    emit player_bases_changed( get_player_bases() );
    emit player_tanks_changed( get_player_tanks() );
    emit enemy_tanks_changed( get_player_tanks() );
    emit projectiles_changed( get_projectiles() );
    emit announcement_changed( m_announcement );
    emit announcement_visibility_changed( m_announcement_visible );
    emit remaining_frags_changed( get_remaining_frags() );
    emit player_remaining_lifes_changed( get_player_remaining_lifes() );
    emit base_remaining_health_changed( get_base_remaining_health() );
    emit explosions_changed( get_explosions() );
}

}// game
