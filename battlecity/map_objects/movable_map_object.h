#ifndef MOVABLE_MAP_OBJECT_H
#define MOVABLE_MAP_OBJECT_H

#include "graphics_map_object.h"

namespace game
{

class movable_map_object : public graphics_map_object,
                           public ecs::event_callback< event::geometry_changed >
{
    Q_OBJECT

public:
    movable_map_object() = default;
    movable_map_object( ecs::entity* entity, const object_type& type, QObject* parent = nullptr );
    ~movable_map_object();

    void set_move_direction( const QString& direction );
    QString get_move_direction() const;

    Q_PROPERTY( QString move_direction READ get_move_direction WRITE set_move_direction )

    // Event callbacks
    void on_event( const event::geometry_changed& event ) override;
};

}// game

#endif
