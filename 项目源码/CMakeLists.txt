cmake_minimum_required(VERSION 3.16)
project(AirWar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# Qt 的安装路径不再写死在本文件里，按以下顺序自动确定：
#   1) 命令行 / CMake GUI 传入：-DCMAKE_PREFIX_PATH="D:/Qt/5.14.2/msvc2017_64"
#   2) 环境变量 QTDIR 指向 Qt 安装目录
#   3) 已加入 PATH 的 Qt
# ---------------------------------------------------------------------------
if(NOT CMAKE_PREFIX_PATH AND DEFINED ENV{QTDIR})
    set(CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

find_package(Qt5 5.12 COMPONENTS Widgets Gui Core QUIET)

if(NOT Qt5_FOUND)
    message(FATAL_ERROR
        "未找到 Qt5（需要 5.12 或更高版本）。请传入 Qt 的安装目录后重新配置，例如：\n"
        "  cmake -S . -B build -DCMAKE_PREFIX_PATH=\"D:/Qt/5.14.2/msvc2017_64\"\n"
        "或者先设置环境变量 QTDIR 指向 Qt 目录。")
endif()

message(STATUS "Found Qt ${Qt5_VERSION} at ${Qt5_DIR}")

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

# 构建后自动部署 Qt 运行时 DLL；找不到 windeployqt 时跳过，不阻断构建
get_filename_component(_qt_bin_dir "${Qt5_DIR}/../../../bin" ABSOLUTE)
find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS "${_qt_bin_dir}")

if(WINDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET airwar POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:airwar>"
        COMMENT "Deploying Qt runtime DLLs..."
    )
else()
    message(STATUS "windeployqt not found - skipping Qt DLL deployment")
endif()
