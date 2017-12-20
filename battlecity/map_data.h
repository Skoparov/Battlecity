#ifndef MAPDATA_H
#define MAPDATA_H

#include <type_traits>

#include <QSize>

#include "map_objects/base_map_object.h"

namespace ecs
{

class world;

}

namespace game
{

class map_data_mediator : public QObject
{
    Q_OBJECT

public:
    map_data_mediator( QObject* parent = nullptr ): QObject( parent ){}

public slots:
    virtual void add_object( const object_type& type, ecs::entity* entity, bool send_update = true ) = 0;
    virtual void remove_all_objects() = 0;

    virtual void entity_hit( const event::entity_hit& ) = 0;
    virtual void entity_killed( const event::entity_killed& ) = 0;
    virtual void entities_removed( const event::entities_removed& ) = 0;

    virtual void prepare_to_load_next_level() = 0;
    virtual void level_started( const QString& level ) = 0;
    virtual void level_completed( const level_game_result& result ) = 0;
    virtual void game_completed() = 0;

signals:
    void start();
    void stop();
    void pause();
    void resume();
    void load_next_level();
};

class map_data final
{
public:
    map_data() = default;
    map_data( const QSize& map_size ) noexcept;

    void set_map_size( const QSize& size ) noexcept;
    void set_map_name( const QString& name );
    int get_rows_count() const noexcept;
    int get_columns_count() const noexcept;
    const QSize& get_map_size() const noexcept;
    const QString& get_map_name() const noexcept;

private:
    QSize m_map_size{};
    QString m_map_name;
};

class game_settings;
void read_map_file( map_data& data,
                    const QString& file,
                    const game_settings& settings,
                    ecs::world& world ,
                    map_data_mediator* mediator = nullptr );

}// game

#endif // MAPDATA_H
