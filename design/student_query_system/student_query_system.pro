QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = student_query_system
TEMPLATE = app

SOURCES += \
    crud.cpp \
    csv.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    sqlexecutor.cpp

HEADERS += \
    crud.h \
    csv.h \
    logindialog.h \
    mainwindow.h \
    sqlexecutor.h
