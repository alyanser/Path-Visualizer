QT += core gui widgets statemachine

CONFIG += \
         c++17 \

SOURCES += \
         main.cpp \
         scene.cpp \ 
         node.cpp \
         helpDialog.cpp

HEADERS += \
         scene.hpp \
         PushButton.hpp \
         node.hpp \ 
         helpDialog.hpp

TARGET = qtGraphAlgos

RESOURCES += \
        resources.qrc