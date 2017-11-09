#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ecs/framework/world.h"
#include "ecs/components.h"

using namespace game;

struct death
{
    ecs::entity_id id{ 0 };
};

class my_subscription : public ecs::event_callback< death >
{
public:
    void on_event( const death& d ) override
    {

    }
};

class health_system : public ecs::system
{
public:
    health_system( ecs::world& w ) : ecs::system( w ){}

    void tick() override
    {
        m_world.for_each< component::health >(
            []( ecs::entity& e, component::health& h )
            {
                //h.
            } );

        death e;
        m_world.emit_event( e );
    }
};

int main(int argc, char *argv[])
{
    ecs::world world;

    health_system system{ world };
    world.add_system( system );

    my_subscription sub;
    world.subscribe< death >( sub );


    ecs::entity& e = world.create_entity();
    ecs::entity& e1 = world.create_entity();

    uint32_t max_health{ 3 };

    e.add_component< component::health >( max_health );
    e1.add_component< component::health >( max_health );



    world.tick();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
