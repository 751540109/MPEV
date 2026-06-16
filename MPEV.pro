QT += core gui widgets sql

TARGET = MPEV
TEMPLATE = app

CONFIG += c++17

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/emailparser.cpp \
    src/database.cpp \
    src/emailmodel.cpp \
    src/timeline.cpp \
    src/contextmenu.cpp \
    src/settingsdialog.cpp

HEADERS += \
    src/mainwindow.h \
    src/emailparser.h \
    src/database.h \
    src/emailmodel.h \
    src/timeline.h \
    src/contextmenu.h \
    src/settingsdialog.h

RESOURCES += \
    resources/resources.qrc

# Windows 特定配置
win32 {
    QMAKE_CXXFLAGS += /utf-8
}

# 高 DPI 支持
DEFINES += QT_DEPRECATED_WARNINGS
