TEMPLATE = app
CONFIG += console c++17
CONFIG -= qt

SOURCES += \
        source/barclasses.cpp \
        source/barcodeCreator.cpp \
        source/component.cpp \
        source/hole.cpp

HEADERS += \
    detection.h \
    include/barclasses.h \
    include/barcodeCreator.h \
    include/barline.h \
    include/barstrucs.h \
    include/component.h \
    include/hole.h \
    include/include_cv.h \
    include/include_py.h \
    include/presets.h

INCLUDEPATH += pathToOpenCV/Include
DEPENDPATH += pathToOpenCV/Include

INCLUDEPATH += ./include

