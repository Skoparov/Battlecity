#include "controller.h"

#include <thread>

#include <QDir>
#include <QThread>
#include <QFileInfo>

#include "ecs/systems.h"
#include "ecs/framework/details/rw_lock_guard.h"

static constexpr auto map_extension = "bcmap";
static constexpr auto map_dir_path = ":/maps/";

static const uint32_t map_switch_pause_duration{ 2000 };


namespace game
{

controller::controller( const game_settings& settings, ecs::world& world ) :
    m_world( world ),
    m_settings( settings )
{
    m_thread = new QThread{ this };
    m_thread->setObjectName( "ECS thread" );

    m_tick_timer = new QTimer{ nullptr };
    m_tick_timer->setInterval( 1000 / m_settings.get_fps() );
    m_tick_timer->moveToThread( m_thread );

    connect( m_thread, SIGNAL( started() ), m_tick_timer, SLOT( start() ) );
    connect( this, SIGNAL( start_tick_timer_signal() ), m_tick_timer, SLOT( start() ) );
    connect( this, SIGNAL( stop_tick_timer_signal() ), m_tick_timer, SLOT( stop() ) );
    connect( m_tick_timer, SIGNAL( timeout() ), this, SLOT( tick() ), Qt::DirectConnection );

    qRegisterMetaType< ecs::entity* >( "ecs::entity*" );
    qRegisterMetaType< game::object_type >( "object_type" );
    qRegisterMetaType< game::level_game_result >( "level_game_result" );
    qRegisterMetaType< event::entity_hit >( "event::entity_hit" );
    qRegisterMetaType< event::entity_killed >( "event::entity_killed" );
    qRegisterMetaType< event::entities_removed >( "event::entities_removed" );
}

controller::~controller()
{
    stop();
}

void controller::init()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    QDir map_dir{ map_dir_path };
    map_dir.setNameFilters( QStringList{} <<  QString{ "*.%1" }.arg( map_extension ) );
    m_levels = map_dir.entryList( QDir::NoDotAndDotDot | QDir::AllEntries );

    if( m_levels.empty() )
    {
        throw std::logic_error{ "No maps found" };
    }

    load_level();

    std::unique_ptr< ecs::system > vic_def_system{
        new system::win_defeat_system{ m_settings.get_base_kills_to_win(), m_world } };

    // create systems
    std::unique_ptr< ecs::system > move_system{ new system::movement_system{ m_world } };
    std::unique_ptr< ecs::system > powerup_system{ new system::powerup_system{ m_world } };

    std::unique_ptr< ecs::system > proj_system{
        new system::projectile_system{ m_settings.get_projectile_size(),
                                       m_settings.get_projectile_damage(),
                                       m_settings.get_projectile_speed(),
                                       m_world } };

    std::unique_ptr< ecs::system > respawn_system{ new system::respawn_system{  m_world } };

    std::unique_ptr< ecs::system > tank_ai_system{
        new system::tank_ai_system{ m_settings.get_ai_chance_to_fire(),
                                    m_settings.get_ai_chance_to_change_direction(),
                                    m_world } };

    std::unique_ptr< ecs::system > animation_system{ new system::animation_system{ m_world } };
    system::animation_system* anim_sys = dynamic_cast< system::animation_system* >( animation_system.get() );
    for( const auto& data_pair : m_settings.get_animation_data() )
    {
        anim_sys->add_animation_settings( data_pair.first, data_pair.second );
    }

    m_systems.emplace_back( std::move( vic_def_system ) );
    m_systems.emplace_back( std::move( move_system ) );
    m_systems.emplace_back( std::move( powerup_system ) );
    m_systems.emplace_back( std::move( animation_system ) );
    m_systems.emplace_back( std::move( proj_system ) );
    m_systems.emplace_back( std::move( respawn_system ) );
    m_systems.emplace_back( std::move( tank_ai_system ) );

    for( auto& system : m_systems )
    {
        m_world.add_system( *system );
    }

    m_world.subscribe< event::level_completed >( *this );
    m_world.subscribe< event::projectile_fired >( *this );
    m_world.subscribe< event::entities_removed >( *this );
    m_world.subscribe< event::entity_killed >( *this );
    m_world.subscribe< event::entity_hit >( *this );
    m_world.subscribe< event::animation_started >( *this );

    m_state = controller_state::stopped;
}

void controller::start()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    m_thread->start();
    emit level_started_signal( m_map_data.get_map_name() );
    m_state = controller_state::running;
}

void controller::pause()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    emit stop_tick_timer_signal();
    m_state = controller_state::paused;
}

void controller::resume()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    emit start_tick_timer_signal();
    m_state = controller_state::running;
}

void controller::start_level()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    m_world.reset();
    load_level();

    if( m_mediator )
    {
        connect( this,
                 SIGNAL( add_object_signal( game::object_type,ecs::entity* ) ),
                 m_mediator,
                 SLOT( add_object( game::object_type, ecs::entity* ) ) );
    }

    emit level_started_signal( m_map_data.get_map_name() );
    emit start_tick_timer_signal();
    m_state = controller_state::running;
}

void controller::stop()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    emit stop_tick_timer_signal();
    if( m_thread->isRunning() )
    {
        m_thread->quit();
        m_thread->wait();
    }

    m_state = controller_state::stopped;
}

QString get_map_path( const QString& map_name )
{
    return QString{ map_dir_path } + map_name;
}

void controller::load_level()
{
    read_map_file( m_map_data,
                   get_map_path( m_levels.front() ),
                   m_settings,
                   m_world,
                   m_mediator );

    for( auto& system : m_systems )
    {
        system->init();
    }
}

const controller_state& controller::get_state() const noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };
    return m_state;
}

void controller::set_map_mediator( map_data_mediator* mediator ) noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };

    m_mediator = mediator;

    connect( this,
             SIGNAL( level_started_signal( const QString& ) ),
             mediator,
             SLOT( level_started( const QString& ) ), Qt::QueuedConnection );

    connect( this,
             SIGNAL( level_completed_signal( const level_game_result& ) ),
             mediator,
             SLOT( level_completed( const level_game_result& ) ), Qt::QueuedConnection );

    connect( this,
             SIGNAL( game_completed_signal() ),
             mediator,
             SLOT( game_completed() ), Qt::QueuedConnection );

    connect( this,
             SIGNAL( entity_hit_signal( const event::entity_hit& ) ),
             mediator,
             SLOT( entity_hit( const event::entity_hit& ) ) );

    connect( this,
             SIGNAL( add_object_signal( game::object_type,ecs::entity* ) ),
             m_mediator,
             SLOT( add_object( game::object_type, ecs::entity* ) ) );

    connect( this,
             SIGNAL( entity_killed_signal( const event::entity_killed& ) ),
             mediator,
             SLOT( entity_killed( const event::entity_killed& ) ) );

    connect( this,
             SIGNAL( entities_removed_signal( const event::entities_removed& ) ),
             mediator,
             SLOT( entities_removed( const event::entities_removed& ) ) );

    connect( this,
             SIGNAL( prepare_to_load_next_level_signal() ),
             mediator,
             SLOT( prepare_to_load_next_level() ) );

    connect( mediator,
             SIGNAL( load_next_level() ),
             this,
             SLOT( start_level() ) );

    connect( mediator,
             SIGNAL( start() ),
             this,
             SLOT( start() ) );

    connect( mediator,
             SIGNAL( stop() ),
             this,
             SLOT( stop() ) );

    connect( mediator,
             SIGNAL( pause() ),
             this,
             SLOT( pause() ) );

    connect( mediator,
             SIGNAL( resume() ),
             this,
             SLOT( resume() ) );
}

int controller::get_rows_num() const noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };
    return m_map_data.get_rows_count();
}

int controller::get_columns_num() const noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };
    return m_map_data.get_columns_count();
}

int controller::get_tile_width() const noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };
    return m_settings.get_tile_size().width();
}

int controller::get_tile_height() const noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };
    return m_settings.get_tile_size().height();
}

const QString& controller::get_level() const noexcept
{
    std::lock_guard< std::mutex > l{ m_mutex };
    return m_map_data.get_map_name();
}

uint32_t controller::get_player_remaining_lifes()
{
    uint32_t lifes_num{ 0 };

    auto players =  m_world.get_entities_with_component< component::player >();
    if( !players.empty() )
    {
        ecs::entity* player{ players.front() };
        component::lifes& lifes = player->get_component< component::lifes >();
        ecs::rw_lock_guard< ecs::rw_lock > l{ lifes, ecs::lock_mode::read };
        lifes_num = lifes.get_lifes();
    }

    return lifes_num;
}

uint32_t controller::get_base_remaining_health()
{
    uint32_t remaining_health{ 0 };

    auto player_bases =  m_world.get_entities_with_component< component::player_base >();
    if( !player_bases.empty() )
    {
        ecs::entity* player_base{ player_bases.front() };
        component::health& health = player_base->get_component< component::health >();
        ecs::rw_lock_guard< ecs::rw_lock > l{ health, ecs::lock_mode::read };

        remaining_health = health.get_health();
    }

    return remaining_health;

}

void controller::on_event( const event::level_completed& event )
{
    if( m_mediator )
    {
        disconnect( this,
                 SIGNAL( add_object_signal( game::object_type,ecs::entity* ) ),
                 m_mediator,
                 SLOT( add_object( game::object_type, ecs::entity* ) ) );
    }

    pause();

    if( event.get_result() == level_game_result::victory )
    {
        m_levels.pop_front();
    }

    if( !m_levels.empty() )
    {
        emit level_completed_signal( event.get_result() );
        // Give the player some time to figure out what's happening, show victory/defeat
        std::this_thread::sleep_for( std::chrono::milliseconds{ map_switch_pause_duration } );
        emit prepare_to_load_next_level_signal();
    }
    else
    {
        emit game_completed_signal();
    }
}

void controller::on_event( const event::projectile_fired& event )
{
    emit add_object_signal( object_type::projectile, &event.get_projectile() );
}

void controller::on_event( const event::animation_started& event )
{
    emit add_object_signal( object_type::animation, event.get_cause_entity() );
}

void controller::on_event( const event::entity_hit& event )
{
    emit entity_hit_signal( event );
}

void controller::on_event(const event::entity_killed &event)
{
    emit entity_killed_signal( event );
}

void controller::on_event( const event::entities_removed& event )
{
    emit entities_removed_signal( event );
}

void controller::tick()
{
    m_world.tick();
}

}// game
