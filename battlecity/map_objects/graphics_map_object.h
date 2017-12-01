#ifndef GRAPHICS_MAP_OBJECT_H
#define GRAPHICS_MAP_OBJECT_H

#include "base_map_object.h"

class graphics_map_object : public base_map_object
{
    Q_OBJECT

public:
    graphics_map_object( const QString& image_path,
                         const bool& visible,
                         const bool& traversible,
                         QObject* parent = nullptr );

    void set_image_path( const QString& path );
    const QString& get_image_path() const noexcept;

    void set_visible( bool visible ) noexcept;
    bool get_visible() const noexcept;

    Q_PROPERTY( QString image_path READ get_image_path CONSTANT )
    Q_PROPERTY( bool visible READ get_visible WRITE set_visible NOTIFY visible_changed )

signals:
    void visible_changed( bool );

private:
    QString m_image_path;
    bool m_visible{ true };
};

#endif
