#ifndef QML_MAP_INTERFACE_H
#define QML_MAP_INTERFACE_H

#include <QTimer>
#include <QQmlListProperty>

#include "map_data.h"
#include "map_objects/tank_map_object.h"

namespace std
{

template<> struct hash< game::object_type >
{
    using argument_type = game::object_type;
    using underlying_type = std::underlying_type< argument_type >::type;
    using result_type = std::hash< underlying_type >::result_type;

    size_t operator()( const argument_type& arg ) const
    {
        std::hash< underlying_type > hasher;
        return hasher( static_cast< underlying_type >( arg ) );
    }
};

}// std

namespace game
{

class controller;

class qml_map_interface : public QObject,
                          public map_data_mediator,
                          public ecs::event_callback< event::projectile_fired >,
                          public ecs::event_callback< event::entities_removed >,
                          public ecs::event_callback< event::enemy_killed >,
                          public ecs::event_callback< event::player_killed >
{
    Q_OBJECT

public:
    qml_map_interface( controller& controller, QObject* parent = nullptr );
    ~qml_map_interface() override;

    void add_object( const object_type& type, ecs::entity& entity ) override;
    void remove_all() override;
    void level_started( const QString& level ) override;
    void level_ended( const level_game_result& result ) override;
    void game_ended( const level_game_result& result ) override;

    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;
    int get_tile_width() const noexcept;
    int get_tile_height() const noexcept;
    int get_frag_width() const noexcept;
    QString get_text() const;
    bool get_text_visible() const noexcept;
    int get_remaining_frags_num() const noexcept;
    int get_remaining_lifes_num() const noexcept;

    QQmlListProperty< graphics_map_object > get_tiles();
    QQmlListProperty< graphics_map_object > get_player_bases();
    QQmlListProperty< tank_map_object > get_player_tanks();
    QQmlListProperty< tank_map_object > get_enemy_tanks();
    QQmlListProperty< movable_map_object > get_projectiles();
    QQmlListProperty< graphics_map_object > get_remaining_frags();

    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > tiles READ get_tiles NOTIFY tiles_changed )
    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > player_bases READ get_player_bases NOTIFY player_bases_changed )
    Q_PROPERTY( QQmlListProperty< game::tank_map_object > player_tanks READ get_player_tanks NOTIFY player_tanks_changed )
    Q_PROPERTY( QQmlListProperty< game::tank_map_object > enemy_tanks READ get_enemy_tanks NOTIFY enemy_tanks_changed )
    Q_PROPERTY( QQmlListProperty< game::movable_map_object > projectiles READ get_projectiles NOTIFY projectiles_changed )
    Q_PROPERTY( QQmlListProperty< game::graphics_map_object > remaining_frags READ get_remaining_frags NOTIFY remaining_frags_changed )
    Q_PROPERTY( int rows_num READ get_rows_count CONSTANT )
    Q_PROPERTY( int columns_num READ get_columns_count CONSTANT )
    Q_PROPERTY( int tile_width READ get_tile_width CONSTANT )
    Q_PROPERTY( int tile_height READ get_tile_height CONSTANT )
    Q_PROPERTY( int frag_width READ get_frag_width CONSTANT )
    Q_PROPERTY( QString announcement READ get_text NOTIFY announcement_changed )
    Q_PROPERTY( bool announcement_visible READ get_text_visible NOTIFY announcement_visibility_changed )
    Q_PROPERTY( int remaining_frags_num READ get_remaining_frags_num CONSTANT )
    Q_PROPERTY( int remaining_lifes READ get_remaining_lifes_num NOTIFY remaining_lifes_changed )

    void on_event( const event::projectile_fired& event ) override;
    void on_event( const event::player_killed& ) override;
    void on_event( const event::enemy_killed& event ) override;
    void on_event( const event::entities_removed& event ) override;

signals:
    void tiles_changed( QQmlListProperty< game::graphics_map_object > );
    void player_bases_changed( QQmlListProperty< game::graphics_map_object > );
    void player_tanks_changed( QQmlListProperty< game::tank_map_object > );
    void enemy_tanks_changed( QQmlListProperty< game::tank_map_object > );
    void projectiles_changed( QQmlListProperty< game::movable_map_object > );
    void remaining_frags_changed( QQmlListProperty< game::graphics_map_object > );
    void remaining_lifes_changed( int );
    void announcement_changed( QString );
    void announcement_visibility_changed( bool );

public slots:
    void hide_announcement();

private:
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


    using object_list = std::list< std::unique_ptr< base_map_object > >;
    std::unordered_map< object_type, object_list > m_map_objects;

    QString m_announcement;
    bool m_announcement_visible{ false };
    QTimer* m_hide_announcement_timer{ nullptr };
};

}// game

#endif // MAPINTERFACE_H
