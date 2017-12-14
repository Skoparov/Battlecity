#ifndef BASE_MAP_OBJECT_H
#define BASE_MAP_OBJECT_H

#include <QObject>

#include "ecs/events.h"
#include "ecs/general_enums.h"
#include "ecs/framework/world.h"

#include <cassert>

namespace game
{

class base_map_object : public QObject,
                        public ecs::event_callback< event::geometry_changed >
{
    Q_OBJECT

public:
    base_map_object() = default;
    base_map_object( ecs::entity* entity, const object_type& type, QObject* parent = nullptr );

    ecs::entity_id get_id() const noexcept;

    void set_position_x( int x ) noexcept;
    int get_position_x() const noexcept;

    void set_position_y( int y ) noexcept;
    int get_position_y() const noexcept;

    int get_width() const noexcept;
    int get_height() const noexcept;

    void set_rotation( int rotation ) noexcept;
    int get_rotation() const noexcept;

    bool get_traversible() const noexcept;

    const object_type& get_type() const noexcept;

    Q_PROPERTY( ecs::numeric_id object_id READ get_id CONSTANT )
    Q_PROPERTY( int pos_x READ get_position_x WRITE set_position_x NOTIFY pos_x_changed )
    Q_PROPERTY( int pos_y READ get_position_y WRITE set_position_y NOTIFY pos_y_changed )
    Q_PROPERTY( int width READ get_width CONSTANT )
    Q_PROPERTY( int height READ get_height CONSTANT )
    Q_PROPERTY( int rotation READ get_rotation WRITE set_rotation NOTIFY rotation_changed )
    Q_PROPERTY( bool traversible READ get_traversible CONSTANT )

    // Event callbacks
    void on_event( const event::geometry_changed& event ) override;

signals:
    void pos_x_changed( int );
    void pos_y_changed( int );
    void rotation_changed( int );

protected:
    ecs::entity* m_entity{ nullptr };
    object_type m_object_type;
};

}// game

#endif
