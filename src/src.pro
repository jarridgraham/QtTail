TEMPLATE = app

FORMS += mainwindow.ui
SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += mdichild.cpp
SOURCES += tail.cpp
SOURCES += highlighter.cpp
SOURCES += genericfilter.cpp
HEADERS += mainwindow.h
HEADERS += mdichild.h
HEADERS += tail.h
HEADERS += highlighter.h
HEADERS += genericfilter.h
MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build

DESTDIR = build
