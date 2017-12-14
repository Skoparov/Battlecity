#ifndef GRAPHICS_MAP_OBJECT_H
#define GRAPHICS_MAP_OBJECT_H

#include "base_map_object.h"

namespace game
{

class graphics_map_object : public base_map_object,
                            public ecs::event_callback< event::graphics_changed >
{
    Q_OBJECT

public:
    graphics_map_object() = default;
    graphics_map_object( ecs::entity* entity, const object_type& type, QObject* parent = nullptr );
    ~graphics_map_object() override;

    void set_image_path( const QString& path );
    const QString& get_image_path() const noexcept;

    void set_visible( bool visible ) noexcept;
    bool get_visible() const noexcept;

    Q_PROPERTY( QString image_path READ get_image_path CONSTANT )
    Q_PROPERTY( bool visible READ get_visible WRITE set_visible NOTIFY visibility_changed )

    // Event callbacks
    void on_event( const event::graphics_changed& event ) override;

signals:
    void image_changed( QString );
    void visibility_changed( bool );
};

}// game

#endif
