#ifndef MAPINTERFACE_H
#define MAPINTERFACE_H

#include <QObject>
#include <QQmlListProperty>

class MapInterface : public QObject
{
public:
    explicit MapInterface( QObject* parent = nullptr );
};

#endif // MAPINTERFACE_H
