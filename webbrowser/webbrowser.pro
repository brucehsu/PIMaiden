# -------------------------------------------------
# Project created by QtCreator 2009-12-08T14:23:25
# -------------------------------------------------
QT += sql
QT += webkit
QT += network
TARGET = webbrowser
TEMPLATE = app
CONFIG += moc
SOURCES += main.cpp \
    webbrowser.cpp \
    omnibox.cpp \
    dbmanager.cpp
HEADERS += webbrowser.h \
    dbmanager.h \
    omnibox.h
RESOURCES += htmls/htmls.qrc \
    icons/icons.qrc
INCLUDEPATH += .
