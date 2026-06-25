QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = DeepSeekConfigTool
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    deepseekapi.cpp \
    docxhandler.cpp

HEADERS += \
    mainwindow.h \
    deepseekapi.h \
    docxhandler.h
