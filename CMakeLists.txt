cmake_minimum_required(VERSION 3.21)

project(pathVisualizer)

set(CMAKE_CXX_COMPILER clang++-12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -fsanitize=address -fsanitize=undefined -Wall -Wshadow -Wold-style-cast -Wstrict-aliasing -Wdouble-promotion -Wundef -Wnull-dereference -Wfloat-equal -Wconversion -Woverflow -pedantic -pthread -Wextra -fno-omit-frame-pointer")

set(CMAKE_AUTOMOC on)
set(CMAKE_AUTORCC on)
set(CMAKE_AUTOUIC on)

set(SOURCES 
         src/main.cxx
         src/node.cxx
         src/pushButton.cxx
         src/scene.cxx
         src/helpDialog.cxx
         resources.qrc
)

set(MOC_INCLUDES
         include/defines.hpp
         include/helpDialog.hpp
         include/node.hpp
         include/pushButton.hpp
         include/scene.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_INCLUDES})

include_directories("include")

find_package(Qt6 COMPONENTS Gui Core Widgets StateMachine REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
         Qt6::Gui
         Qt6::Core
         Qt6::Widgets
         Qt6::StateMachine
)