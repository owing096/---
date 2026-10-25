cmake_minimum_required(VERSION 3.16)
project(AirWar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt 5.14.2 MinGW 64-bit path
set(CMAKE_PREFIX_PATH "D:/QT/5.14.2/msvc2017_64")

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui Core)

add_executable(airwar
    main.cpp
    defs.h
    game.h game.cpp
    menuwidget.h menuwidget.cpp
    gameoverwidget.h gameoverwidget.cpp
    helpwidget.h helpwidget.cpp
    gamescene.h gamescene.cpp
    player.h player.cpp
    bullet.h bullet.cpp
    enemy.h enemy.cpp
    powerup.h powerup.cpp
    assets.qrc
)

target_link_libraries(airwar PRIVATE Qt5::Widgets Qt5::Gui Qt5::Core)

# 构建后自动部署 Qt DLL 到输出目录
add_custom_command(TARGET airwar POST_BUILD
    COMMAND "D:/QT/5.14.2/msvc2017_64/bin/windeployqt.exe" "$<TARGET_FILE:airwar>"
    COMMENT "Deploying Qt DLLs..."
)
