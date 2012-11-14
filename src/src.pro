TEMPLATE = app

FORMS += mainwindow.ui
FORMS += filters.ui
FORMS += filtersconfig.ui
FORMS += find.ui
FORMS += about.ui

SOURCES += main.cpp
SOURCES += mainwindow.cpp
SOURCES += mdichild.cpp
SOURCES += tail.cpp
SOURCES += highlighter.cpp
SOURCES += genericfilter.cpp
SOURCES += newfilter.cpp
SOURCES += filterconfig.cpp
SOURCES += filtervalidator.cpp
SOURCES += filtermodel.cpp
SOURCES += format.cpp
SOURCES += findwindow.cpp
HEADERS += mainwindow.h
HEADERS += mdichild.h
HEADERS += tail.h
HEADERS += highlighter.h
HEADERS += genericfilter.h
HEADERS += newfilter.h
HEADERS += filterconfig.h
HEADERS += filtervalidator.h
HEADERS += filtermodel.h
HEADERS += format.h
HEADERS += findwindow.h
HEADERS += about.h
MOC_DIR = build
OBJECTS_DIR = build
UI_DIR = build

DESTDIR = build
