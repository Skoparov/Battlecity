QT += qml quick

CONFIG += c++11

HEADERS += \
# ecs framework
        ecs/framework/entity.h \
        ecs/framework/id_engine.h \
        ecs/framework/world.h \
        ecs/framework/details/polymorph.h \
# game ecs stuff
        ecs/details/spinlock.h \
        ecs/components.h \
        ecs/events.h \
        ecs/systems.h \
        ecs/entity_factory.h \
        ecs/general_enums.h \
# game stuff
        map_objects/base_map_object.h \
        map_objects/graphics_map_object.h \
        map_objects/animated_map_object.h \
        map_objects/movable_map_object.h \
        map_objects/tank_map_object.h \
        map_interface.h \
        controller.h \
        game_settings.h \
        map_data.h


SOURCES += \
        main.cpp \
# ecs framework
        ecs/framework/entity.cpp \
        ecs/framework/id_engine.cpp \
        ecs/framework/world.cpp \
        ecs/framework/details/polymorph.cpp \
        ecs/framework/details/polymorph.impl \
# game ecs stuff
        ecs/details/spinlock.cpp \
        ecs/components.cpp \
        ecs/events.cpp \
        ecs/systems.cpp \
        ecs/entity_factory.cpp \
# game stuff
        map_objects/base_map_object.cpp \
        map_objects/graphics_map_object.cpp \
        map_objects/animated_map_object.cpp \
        map_objects/movable_map_object.cpp \
        map_objects/tank_map_object.cpp \
        map_interface.cpp \
        controller.cpp \
        game_settings.cpp \
        map_data.cpp

RESOURCES += resources.qrc
DEFINES += "ECS_CONCURRENCY"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
