QT += core gui widgets statemachine
CONFIG += \
         c++17 \
         precompile_header

PRECOMPILED_HEADER = ~/Qt/precompiler.hpp

SOURCES += \
         main.cpp \
         scene.cpp \ 
         node.cpp

HEADERS += \
         scene.hpp \
         PushButton.hpp \

TARGET = qtGraphAlgos

RESOURCES += \
        resources.qrc