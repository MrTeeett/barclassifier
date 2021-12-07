TEMPLATE = app
CONFIG += console c++11
CONFIG -= qt

SOURCES += \
        main.cpp \
        source/Barpy.cpp \
        source/barclasses.cpp \
        source/barcodeCreator.cpp \
        source/component.cpp \
        source/hole.cpp

HEADERS += \
    include/barImg.h \
    include/barclasses.h \
    include/barcodeCreator.h \
    include/barline.h \
    include/barstrucs.h \
    include/component.h \
    include/hole.h \
    include/include_cv.h \
    include/include_py.h \
    include/presets.h \
    include/pytemplcalsses.h
