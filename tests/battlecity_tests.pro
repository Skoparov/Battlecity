QT += testlib
QT -= gui

CONFIG += c++11 qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

HEADERS +=../battlecity/ecs/framework/entity.h \
        ../battlecity/ecs/framework/id_engine.h \
        ../battlecity/ecs/framework/world.h \
        ../battlecity/ecs/framework/details/polymorph.h \
        ../battlecity/ecs/framework/details/rw_lock.h \
        ../battlecity/ecs/framework/details/atomic_locks.h \
        ../battlecity/ecs/framework/details/rw_lock_guard.h \
        ../battlecity/ecs/framework/details/rw_lock_modes.h \
        ../battlecity/ecs/framework/details/cpp14/make_unique.h \
        ../battlecity/ecs/framework/details/cpp14/integer_sequence.h

SOURCES +=  tst_ecs_tests.cpp \
        ../battlecity/ecs/framework/entity.cpp \
        ../battlecity/ecs/framework/id_engine.cpp \
        ../battlecity/ecs/framework/world.cpp \
        ../battlecity/ecs/framework/details/polymorph.cpp \
        ../battlecity/ecs/framework/details/polymorph.impl \
        ../battlecity/ecs/framework/details/rw_lock.cpp \
        ../battlecity/ecs/framework/details/atomic_locks.cpp
