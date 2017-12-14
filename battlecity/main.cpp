#include <iostream>

#include <QtQml>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "controller.h"
#include "map_interface.h"
#include "ecs/framework/world.h"

int main(int argc, char *argv[])
{
    int exit_code{ 0 };

    try
    {
        game::game_settings settings{ game::read_game_settings( ":/settings/settings.xml" ) };
        ecs::world world;

        game::controller controller{ settings, world };
        game::qml_map_interface map_interface{ controller };

        controller.set_map_mediator( &map_interface );
        controller.init();

        qmlRegisterType< game::graphics_map_object >();
        qmlRegisterType< game::tank_map_object >();
        qmlRegisterType< game::movable_map_object >();

        QGuiApplication app{ argc, argv };
        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty( "map_interface", &map_interface );
        engine.load( QUrl{ QStringLiteral( "qrc:/qml/main.qml" ) } );

        controller.start();

        exit_code = app.exec();
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        exit_code = 1;
    }

    return exit_code;
}
