cmake_minimum_required(VERSION 3.16)
project(MPEV VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

add_executable(MPEV
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/emailparser.cpp
    src/emailparser.h
    src/database.cpp
    src/database.h
    src/emailmodel.cpp
    src/emailmodel.h
    src/timeline.cpp
    src/timeline.h
    src/contextmenu.cpp
    src/contextmenu.h
    src/settingsdialog.cpp
    src/settingsdialog.h
    resources/resources.qrc
)

target_link_libraries(MPEV PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

set_target_properties(MPEV PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    BUNDLE True
    BUNDLE_NAME "MPEV"
    BUNDLE_GUI "com.mpev.app"
    BUNDLE_VERSION "${PROJECT_VERSION}"
    BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    BUNDLE_COPYRIGHT "Copyright © 2026 MPEV"
)
