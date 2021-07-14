QT += core gui widgets statemachine

CONFIG += \
         c++17 \
         sanitizer \
         sanitize_address \ 
         sanitize_undefined

SOURCES += \
         main.cpp \
         scene.cpp \ 
         node.cpp \
         helpDialog.cpp \
         PushButton.cpp

HEADERS += \
         scene.hpp \
         PushButton.hpp \
         node.hpp \ 
         helpDialog.hpp

TARGET = qtGraphAlgos

RESOURCES += \
        resources.qrc