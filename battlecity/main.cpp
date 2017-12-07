#include <iostream>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "map_interface.h"
#include "ecs/framework/world.h"

using namespace game;

int main(int argc, char *argv[])
{
    int exit_code{ 0 };

    try
    {
        ecs::world world;

        qml_map_interface map_interface{ world };
        map_interface.load_level( 1 );

        QGuiApplication app{ argc, argv };
        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty( "map_interface", &map_interface );
        engine.load( QUrl{ QStringLiteral( "qrc:/qml/main.qml" ) } );


        exit_code = app.exec();
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        exit_code = 1;
    }

    return exit_code;
}
