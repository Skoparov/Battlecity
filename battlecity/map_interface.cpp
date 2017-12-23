#include "map_interface.h"

#include "controller.h"

static constexpr auto pause_resume_button_text_pause = "Pause";
static constexpr auto pause_resume_button_text_resume = "Resume";

static const uint32_t announcement_duration{ 3000 };

namespace game
{

qml_map_interface::qml_map_interface( controller& controller,
                                      QObject* parent ) :
    map_data_mediator( parent ),
    m_controller( controller )
{
    m_hide_announcement_timer = new QTimer{ this };
    connect( m_hide_announcement_timer, SIGNAL( timeout() ), this, SLOT( hide_announcement() ) );
}

qml_map_interface::~qml_map_interface()
{
    remove_all_objects();
}

void qml_map_interface::add_object( const object_type& type, ecs::entity* entity, bool send_update )
{
    std::unique_ptr< base_map_object > map_object;

    switch( type )
    {
    case object_type::tile:
    {
        auto object = std::make_unique< graphics_map_object >( entity, type );
        m_tiles.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::player_tank:
    {
        auto object = std::make_unique< tank_map_object >( entity, type );
        m_player_tanks.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::enemy_tank:
    {
        auto object = std::make_unique< tank_map_object >( entity, type );
        m_enemy_tanks.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::player_base:
    {
        auto object = std::make_unique< graphics_map_object >( entity, type );
        m_player_bases.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::projectile:
    {
        auto object = std::make_unique< movable_map_object >( entity, type );
        m_projectiles.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::frag:
    {
        auto object = std::make_unique< graphics_map_object >( entity, type );
        m_remaining_frags.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::animation:
    {
        auto object = std::make_unique< animated_map_object >( entity, type );
        m_animations.append( object.get() );
        map_object = std::move( object );
        break;
    }
    case object_type::respawn_point: break;
    default:
        assert( false );
    }

    assert( map_object );
    m_map_objects[ type ].emplace( entity->get_id(), std::move( map_object ) );

    if( send_update )
    {
        objects_of_type_changed( type );
    }
}

void qml_map_interface::remove_all_objects()
{
    m_tiles.clear();
    m_player_tanks.clear();
    m_enemy_tanks.clear();
    m_player_bases.clear();
    m_projectiles.clear();
    m_remaining_frags.clear();
    m_projectiles.clear();
    m_animations.clear();

    update_all();

    m_map_objects.clear();
}

void qml_map_interface::prepare_to_load_next_level()
{
    remove_all_objects();
    emit load_next_level();
}

void qml_map_interface::entity_hit( const event::entity_hit& event )
{
    const object_type& victim_type = event.get_victim_type();
    if( victim_type ==  object_type::player_base )
    {
        emit base_remaining_health_changed( m_controller.get_base_remaining_health() );
    }
}

void qml_map_interface::entity_killed( const event::entity_killed& event )
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

void qml_map_interface::entities_removed( const event::entities_removed& event )
{
    std::unordered_map< object_type,
                        std::list< std::unique_ptr< base_map_object > > > objects_to_remove;

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

void qml_map_interface::level_started( const QString& level )
{
    update_pause_resume_button_state( true );
    update_announcement( QString{ "Level %1" }.arg( level ), false );
    m_hide_announcement_timer->start( announcement_duration );

    update_all();
}

void qml_map_interface::level_completed( const level_game_result& result )
{
    update_pause_resume_button_state( false );
    update_announcement( QString{ "%1" }.arg( result == level_game_result::victory?
                                                  "Victory" : "Defeat" ),
                         true );
}

void qml_map_interface::game_completed()
{
    update_pause_resume_button_state( false );
    update_announcement( QString{ "Game completed" }, true );
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
    return m_announcement_text;
}

bool qml_map_interface::get_announecement_visible() const noexcept
{
    return m_announcement_visible;
}

QString qml_map_interface::get_pause_resume_button_text() const
{
    return m_controller.get_state() == controller_state::paused?
                pause_resume_button_text_resume : pause_resume_button_text_pause;
}

bool qml_map_interface::get_pause_resume_button_visible() const noexcept
{
    return m_pause_play_button_visible;
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

QQmlListProperty< animated_map_object > qml_map_interface::get_animations()
{
    return QQmlListProperty< animated_map_object >{ this, m_animations };
}

void qml_map_interface::pause_resume()
{
    const controller_state& state = m_controller.get_state();
    if( state == controller_state::paused )
    {
        emit resume();
    }
    else if( state == controller_state::running )
    {
        emit pause();
    }

    emit pause_resume_button_text_changed( get_pause_resume_button_text() );
}

void qml_map_interface::hide_announcement()
{
    m_hide_announcement_timer->stop();
    m_announcement_visible = false;
    emit announcement_visibility_changed( m_announcement_visible );
}

void qml_map_interface::update_announcement( const QString& text, bool send_update )
{
    m_announcement_text = text;
    m_announcement_visible = true;

    if( send_update )
    {
        emit announcement_text_changed( m_announcement_text );
        emit announcement_visibility_changed( m_announcement_visible );
    }
}

void qml_map_interface::update_pause_resume_button_state( bool visible )
{
    m_pause_play_button_visible = visible;
    emit pause_resume_button_text_changed( get_pause_resume_button_text() );
    emit pause_resume_button_visibility_changed( m_pause_play_button_visible );
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
        break;
    case object_type::frag:
        emit remaining_frags_changed( get_remaining_frags() );
        break;
    case object_type::animation:
        emit animations_changed( get_animations() );
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
    case object_type::animation:
    {
        m_animations.erase( std::remove( m_animations.begin(), m_animations.end(), obj ),
                             m_animations.end() );
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
    emit announcement_text_changed( m_announcement_text );
    emit announcement_visibility_changed( m_announcement_visible );
    emit remaining_frags_changed( get_remaining_frags() );
    emit player_remaining_lifes_changed( get_player_remaining_lifes() );
    emit base_remaining_health_changed( get_base_remaining_health() );
    emit animations_changed( get_animations() );
}

}// game
