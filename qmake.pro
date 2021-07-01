QT += core gui widgets statemachine

CONFIG += \
         c++17 \

SOURCES += \
         main.cpp \
         scene.cpp \ 
         node.cpp

HEADERS += \
         scene.hpp \
         PushButton.hpp \
         node.hpp

TARGET = qtGraphAlgos

RESOURCES += \
        resources.qrc