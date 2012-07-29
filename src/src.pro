TEMPLATE = app

FORMS += mainwindow.ui
SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += mdichild.cpp
SOURCES += tail.cpp
HEADERS += mainwindow.h
HEADERS += mdichild.h
HEADERS += tail.h
MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build

DESTDIR = build
