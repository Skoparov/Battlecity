#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <mutex>

#include <QTimer>

#include "map_data.h"
#include "ecs/events.h"
#include "game_settings.h"

namespace game
{

enum class controller_state{ unintialized, stopped, paused, running };

// A general class that handles all game logic.
// It's not related to graphics, though it may supply the

class controller :  public QObject,
                    public ecs::event_callback< event::level_completed >,
                    public ecs::event_callback< event::projectile_fired >,
                    public ecs::event_callback< event::entities_removed >,
                    public ecs::event_callback< event::entity_killed >,
                    public ecs::event_callback< event::entity_hit >,
                    public ecs::event_callback< event::animation_started >
{
    Q_OBJECT

public:
    controller( const game_settings& settings, ecs::world& world );
    ~controller();

    void init();

    const controller_state& get_state() const noexcept;

    void set_map_mediator( map_interface* mediator ) noexcept;

    template< typename event_type >
    void emit_event( const event_type& event )
    {
        m_world.emit_event( event );
    }

    int get_rows_num() const noexcept;
    int get_columns_num() const noexcept;
    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;
    const QString& get_level() const noexcept;
    uint32_t get_player_remaining_lifes();
    uint32_t get_base_remaining_health();

    void on_event( const event::level_completed& event ) override;
    void on_event( const event::projectile_fired& event ) override;
    void on_event( const event::animation_started& event ) override;
    void on_event( const event::entity_hit& event ) override;
    void on_event( const event::entity_killed& event ) override;
    void on_event( const event::entities_removed& event ) override;

public slots:
    void start();
    void stop();
    void pause();
    void resume();
    void start_level();

private:
    void load_level();

private slots:
    void tick();

signals:
    // mediator
    void level_started_signal( const QString& );
    void level_completed_signal( const level_game_result& );
    void game_completed_signal();
    void add_object_signal( const object_type&, ecs::entity* );
    void entity_hit_signal( const event::entity_hit& );
    void entity_killed_signal( const event::entity_killed& );
    void entities_removed_signal( const event::entities_removed& );
    void prepare_to_load_next_level_signal();

    //internal
    void stop_tick_timer_signal();
    void start_tick_timer_signal();

private:
    ecs::world& m_world;
    map_data m_map_data;
    game_settings m_settings;
    map_interface* m_mediator{ nullptr };

    QStringList m_levels;

    QTimer* m_tick_timer{ nullptr };
    QThread* m_thread{ nullptr };
    std::list< std::unique_ptr< ecs::system > > m_systems;

    controller_state m_state{ controller_state::unintialized };

    mutable std::mutex m_mutex;
};

}// game


Q_DECLARE_METATYPE( ecs::entity* )
Q_DECLARE_METATYPE( game::object_type )
Q_DECLARE_METATYPE( game::level_game_result )
Q_DECLARE_METATYPE( game::event::entity_hit )
Q_DECLARE_METATYPE( game::event::entity_killed )
Q_DECLARE_METATYPE( game::event::entities_removed )

#endif // CONTROLLER_H
