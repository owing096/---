QT       += core gui widgets
CONFIG   += c++17

TARGET    = airwar
TEMPLATE  = app

SOURCES += \
    main.cpp \
    game.cpp \
    menuwidget.cpp \
    gameoverwidget.cpp \
    helpwidget.cpp \
    gamescene.cpp \
    player.cpp \
    bullet.cpp \
    enemy.cpp \
    powerup.cpp

HEADERS += \
    defs.h \
    game.h \
    menuwidget.h \
    gameoverwidget.h \
    helpwidget.h \
    gamescene.h \
    player.h \
    bullet.h \
    enemy.h \
    powerup.h

RESOURCES += assets.qrc
