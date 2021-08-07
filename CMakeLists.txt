cmake_minimum_required(VERSION 3.0)

project(algorithmVisualizer)

set(CMAKE_CXX_COMPILER clang++-12)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -fsanitize=address -fsanitize=undefined -Wall -Wshadow -Wold-style-cast -Wstrict-aliasing -Wdouble-promotion -Wundef -Wnull-dereference -Wfloat-equal -Wconversion -Woverflow -pedantic -pthread -Wextra -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall -Wshadow -Wold-style-cast -Wstrict-aliasing -Wfloat-equal -Wconversion -Wdouble-promotion -Wundef -Wfatal-errors -Woverflow -pedantic -pthread -Wextra")

set(CMAKE_AUTOMOC true)
set(CMAKE_AUTORCC true)

add_executable(${PROJECT_NAME}
         main.cpp
         node.cpp
         pushButton.cpp
         scene.cpp
         helpDialog.cpp
         resources.qrc
)

find_package(Qt6 COMPONENTS Gui Core Widgets StateMachine REQUIRED)

target_link_libraries(${PROJECT_NAME}
         PRIVATE 
                  Qt6::Gui
                  Qt6::Core
                  Qt6::Widgets
                  Qt6::StateMachine
)