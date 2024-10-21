cmake_minimum_required(VERSION 3.18)

project(pathVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)
set(CMAKE_AUTOUIC on)

set(SOURCES 
         src/main.cc
         src/node.cc
         src/scene.cc
         src/helpDialog.cc
         resources.qrc
)

set(MOC_INCLUDES
         include/helpDialog.h
         include/node.h
         include/pushButton.h
         include/scene.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_INCLUDES})

target_include_directories(${PROJECT_NAME} PRIVATE
         "include"
)

find_package(Qt6 COMPONENTS Core Widgets StateMachine REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
         Qt6::Core
         Qt6::Widgets
         Qt6::StateMachine
)