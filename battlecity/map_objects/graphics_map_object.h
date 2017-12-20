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

    const QString& get_image_path() const noexcept;

    bool get_visible() const noexcept;

    Q_PROPERTY( QString image_path READ get_image_path NOTIFY image_changed )
    Q_PROPERTY( bool visible READ get_visible NOTIFY visibility_changed )

    // Event callbacks
    void on_event( const event::graphics_changed& event ) override;

signals:
    void image_changed( QString );
    void visibility_changed( bool );
};

}// game

#endif
