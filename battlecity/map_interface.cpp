#include "map_interface.h"

#include <unordered_set>

#include "controller.h"



namespace game
{

qml_map_interface::qml_map_interface( controller& controller,
                                      QObject* parent ) :
    QObject( parent ),
    m_controller( controller )
{
    m_controller.subscribe< event::projectile_fired >( *this );
    m_controller.subscribe< event::entities_removed >( *this );
    m_controller.subscribe< event::enemy_killed >( *this );
    m_controller.subscribe< event::player_killed >( *this );
    m_controller.subscribe< event::explosion_started >( *this );

    m_hide_announcement_timer = new QTimer{ this };
    connect( m_hide_announcement_timer, SIGNAL( timeout() ), this, SLOT( hide_announcement() ) );
}

qml_map_interface::~qml_map_interface()
{
    m_controller.unsubscribe< event::projectile_fired >( *this );
    m_controller.unsubscribe< event::entities_removed >( *this );
    m_controller.unsubscribe< event::enemy_killed >( *this );
    m_controller.unsubscribe< event::player_killed >( *this );
    m_controller.unsubscribe< event::explosion_started >( *this );
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

    m_map_objects[ type ].emplace_back( std::move( map_object ) );
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

    update_all();

    m_map_objects.clear();
}

void qml_map_interface::level_started( const QString& level )
{
    m_announcement = QString{ "Level %1" }.arg( level );
    update_all();
}

void qml_map_interface::level_ended( const level_game_result& result )
{
    m_announcement_visible = true;
    m_announcement = QString{ "%1" }.arg( result == level_game_result::victory?
                                              "Victory" : "Defeat" );

    emit announcement_changed( m_announcement );
    emit announcement_visibility_changed( m_announcement_visible );
}

void qml_map_interface::game_ended( const level_game_result& result )
{
    m_announcement_visible = true;
    m_announcement = QString{ "Game %1" }.arg( result == level_game_result::victory?
                                      "completed" : "lost" );

    emit announcement_changed( m_announcement );
    emit announcement_visibility_changed( m_announcement_visible );
}

int qml_map_interface::get_rows_count() const noexcept
{
    return m_controller.get_rows_count();
}

int qml_map_interface::get_columns_count() const noexcept
{
    return m_controller.get_columns_count();
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

QString qml_map_interface::get_text() const
{
    return m_announcement;
}

bool qml_map_interface::get_text_visible() const noexcept
{
    return m_announcement_visible;
}

int qml_map_interface::get_remaining_frags_num() const noexcept
{
    return m_remaining_frags.size();
}

int qml_map_interface::get_remaining_lifes_num() const noexcept
{
    return m_controller.get_remaining_lifes();
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
    add_object( object_type::projectile, event.get_projectile() );
    objects_of_type_changed( object_type::projectile );
}

void qml_map_interface::on_event( const event::player_killed& )
{
    emit remaining_lifes_changed( m_controller.get_remaining_lifes() );
}

void qml_map_interface::on_event( const event::enemy_killed& )
{
    objects_of_type_changed( object_type::frag );
}

void qml_map_interface::on_event( const event::entities_removed& event )
{
    const auto& entities = event.get_cause_entities();
    std::unordered_map< object_type, object_list > objects_to_remove;

    for( auto it = m_map_objects.begin(); it != m_map_objects.end(); ++it )
    {
        const object_type& type = it->first;
        auto& objects_list = it->second;

        auto object_it = objects_list.begin();
        while( object_it != objects_list.end() )
        {
            if( entities.count( ( *object_it )->get_id() ) )
            {
                objects_to_remove[ type ].emplace_back( std::move( *object_it ) );
                objects_list.erase( object_it++ );
            }
            else
            {
                ++object_it;
            }
        }
    }

    for( const auto& pair_data : objects_to_remove )
    {
        const object_type& type = pair_data.first;

        for( const auto& obj : pair_data.second )
        {
            remove_object_from_model( type, obj.get() );
        }

        objects_of_type_changed( type );
    }
}

void qml_map_interface::on_event( const event::explosion_started& event )
{
    add_object( object_type::explosion, *event.get_cause_entity() );
    objects_of_type_changed( object_type::explosion );
}

void qml_map_interface::animation_ended( unsigned int id )
{
    auto explosions_it = m_map_objects.find( object_type::explosion );
    if( explosions_it != m_map_objects.end() )
    {
        std::unique_ptr< base_map_object > explosion;

        object_list& explosion_list = explosions_it->second;
        auto explosion_it = std::find_if( explosion_list.begin(), explosion_list.end(),
                                          [ id ]( const std::unique_ptr< base_map_object >& obj )
        {
            return obj->get_id() == id;
        } );

        if( explosion_it != explosion_list.end() )
        {
            explosion = std::move( *explosion_it );
            explosion_list.erase( explosion_it );

            remove_object_from_model( object_type::explosion, explosion.get() );
            objects_of_type_changed( object_type::explosion );

            event::explosion_ended event;
            event.set_cause_id( id );
            m_controller.emit_event( event );
        }
    }
}

void qml_map_interface::hide_announcement()
{
    m_hide_announcement_timer->stop();
    m_announcement_visible = false;
    emit announcement_visibility_changed( m_announcement_visible );
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
    emit tiles_changed( get_tiles() );
    emit player_bases_changed( get_player_bases() );
    emit player_tanks_changed( get_player_tanks() );
    emit enemy_tanks_changed( get_player_tanks() );
    emit projectiles_changed( get_projectiles() );
    emit announcement_changed( m_announcement );
    emit announcement_visibility_changed( m_announcement_visible );
    emit remaining_frags_changed( get_remaining_frags() );
    emit explosions_changed( get_explosions() );
}

}// game
