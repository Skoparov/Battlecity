#ifndef QML_MAP_INTERFACE_H
#define QML_MAP_INTERFACE_H

#include <QTimer>
#include <QQmlListProperty>

#include "map_data.h"
#include "map_objects/tank_map_object.h"
#include "map_objects/animated_map_object.h"

namespace game
{

class controller;

class qml_map_interface : public map_data_mediator
{
    Q_OBJECT

public:
    qml_map_interface( controller& controller, QObject* parent = nullptr );
    ~qml_map_interface() override;


public slots:
    void add_object( const object_type& type, ecs::entity* entity, bool send_update = true ) override;
    void remove_all_objects() override;
    void prepare_to_load_next_level() override;

    void entity_hit( const event::entity_hit& ) override;
    void entity_killed( const event::entity_killed& ) override;
    void entities_removed( const event::entities_removed& ) override;

    void level_started( const QString& level ) override;
    void level_completed( const level_game_result& result ) override;
    void game_completed() override;

public:
    int get_rows_num() const noexcept;
    int get_columns_num() const noexcept;
    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;
    int get_frag_width() const noexcept;
    int get_remaining_frags_num() const noexcept;
    int get_player_remaining_lifes() const noexcept;
    int get_base_remaining_health() const noexcept;
    QString get_announcement_text() const;
    bool get_announecement_visible() const noexcept;
    QString get_pause_resume_button_text() const;
    bool get_pause_resume_button_visible() const noexcept;

    QQmlListProperty< graphics_map_object > get_tiles();
    QQmlListProperty< graphics_map_object > get_player_bases();
    QQmlListProperty< tank_map_object > get_player_tanks();
    QQmlListProperty< tank_map_object > get_enemy_tanks();
    QQmlListProperty< movable_map_object > get_projectiles();
    QQmlListProperty< graphics_map_object > get_remaining_frags();
    QQmlListProperty< animated_map_object > get_animations();
    QQmlListProperty< graphics_map_object > get_powerups();

    Q_PROPERTY( int rows_num READ get_rows_num NOTIFY rows_num_changed )
    Q_PROPERTY( int columns_num READ get_columns_num  NOTIFY columns_num_changed )
    Q_PROPERTY( int tile_width READ get_tile_width CONSTANT )
    Q_PROPERTY( int tile_height READ get_tile_height CONSTANT )
    Q_PROPERTY( int frag_width READ get_frag_width CONSTANT )
    Q_PROPERTY( int remaining_frags_num READ get_remaining_frags_num CONSTANT )
    Q_PROPERTY( int player_remaining_lifes READ get_player_remaining_lifes NOTIFY player_remaining_lifes_changed )
    Q_PROPERTY( int base_remaining_health READ get_base_remaining_health NOTIFY base_remaining_health_changed )
    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > tiles READ get_tiles NOTIFY tiles_changed )
    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > player_bases READ get_player_bases NOTIFY player_bases_changed )
    Q_PROPERTY( QQmlListProperty< game::tank_map_object > player_tanks READ get_player_tanks NOTIFY player_tanks_changed )
    Q_PROPERTY( QQmlListProperty< game::tank_map_object > enemy_tanks READ get_enemy_tanks NOTIFY enemy_tanks_changed )
    Q_PROPERTY( QQmlListProperty< game::movable_map_object > projectiles READ get_projectiles NOTIFY projectiles_changed )
    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > remaining_frags READ get_remaining_frags NOTIFY remaining_frags_changed )
    Q_PROPERTY( QQmlListProperty< game::animated_map_object > animations READ get_animations NOTIFY animations_changed )
    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > powerups READ get_powerups() NOTIFY powerups_changed )
    Q_PROPERTY( QString announcement_text READ get_announcement_text NOTIFY announcement_text_changed )
    Q_PROPERTY( bool announcement_visible READ get_announecement_visible NOTIFY announcement_visibility_changed )
    Q_PROPERTY( QString pause_play_button_text READ get_pause_resume_button_text NOTIFY pause_resume_button_text_changed )
    Q_PROPERTY( bool pause_play_button_visible READ get_pause_resume_button_visible NOTIFY pause_resume_button_visibility_changed )

    Q_INVOKABLE void pause_resume();

signals:
    // model
    void rows_num_changed( int );
    void columns_num_changed( int );
    void tiles_changed( QQmlListProperty< game::graphics_map_object > );
    void player_bases_changed( QQmlListProperty< game::graphics_map_object > );
    void player_tanks_changed( QQmlListProperty< game::tank_map_object > );
    void enemy_tanks_changed( QQmlListProperty< game::tank_map_object > );
    void projectiles_changed( QQmlListProperty< game::movable_map_object > );
    void remaining_frags_changed( QQmlListProperty< game::graphics_map_object > );
    void animations_changed( QQmlListProperty< game::animated_map_object > );
    void powerups_changed( QQmlListProperty< game::graphics_map_object > );

    void player_remaining_lifes_changed( int );
    void base_remaining_health_changed( int );
    void announcement_text_changed( const QString& );
    void announcement_visibility_changed( bool );
    void pause_resume_button_text_changed( const QString& );
    void pause_resume_button_visibility_changed( bool );

private slots:
    void hide_announcement();

private:
    void update_announcement( const QString& text, bool send_update );
    void update_pause_resume_button_state( bool visible );
    void objects_of_type_changed( const object_type& type );
    void remove_object_from_model( const object_type& type, base_map_object* obj );
    void update_all();

private:
    controller& m_controller;

    // Buffered data used by qml engine
    QList< graphics_map_object* > m_tiles;
    QList< graphics_map_object* > m_player_bases;
    QList< tank_map_object* > m_player_tanks;
    QList< tank_map_object* > m_enemy_tanks;
    QList< movable_map_object* > m_projectiles;
    QList< graphics_map_object* > m_remaining_frags;
    QList< animated_map_object* > m_animations;
    QList< graphics_map_object* > m_powerups;

    using object_umap = std::unordered_map< ecs::entity_id, std::unique_ptr< base_map_object > >;
    std::unordered_map< object_type, object_umap > m_map_objects;

    QString m_announcement_text;
    bool m_announcement_visible{ false };
    QTimer* m_hide_announcement_timer{ nullptr };

    bool m_pause_play_button_visible{ false };
    bool m_level_running{ false };
};

}// game

#endif // MAPINTERFACE_H
