#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QTimer>

#include "map_data.h"
#include "ecs/events.h"
#include "game_settings.h"

namespace game
{

enum class controller_state{ unintialized, stopped, paused, running };

class controller : public QObject,
                   public ecs::event_callback< event::level_completed >
{
    Q_OBJECT

public:
    controller( const game_settings& settings, ecs::world& world );

    void init();

    void start();
    void pause();
    void resume();
    void stop();

    const controller_state& get_state() const noexcept;

    void set_map_mediator( map_data_mediator* mediator ) noexcept;

    template< typename event_type >
    void subscribe( event_callback< event_type >& callback )
    {
        m_world.subscribe< event_type >( callback );
    }

    template< typename event_type >
    void unsubscribe( event_callback< event_type >& callback )
    {
        m_world.unsubscribe< event_type >( callback );
    }

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

    virtual void on_event( const event::level_completed& event );

private:
    void load_level();

public slots:
    void tick();

private:
    ecs::world& m_world;
    map_data m_map_data;
    game_settings m_settings;
    map_data_mediator* m_mediator{ nullptr };

    QStringList m_levels;
    bool m_need_to_load_level{ true };

    QTimer* m_tick_timer{ nullptr };
    std::list< std::unique_ptr< ecs::system > > m_systems;

    controller_state m_state{ controller_state::unintialized };
};

}// game

#endif // CONTROLLER_H
