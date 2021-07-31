cmake_minimum_required(VERSION 3.0)

project(algorithmVisualizer)

set(CMAKE_AUTOMOC true)
set(CMAKE_AUTORCC true)

set(CMAKE_CXX_COMPILER clang++-10)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "-Wdouble-promotion -Wundef -Wall  -Wshadow -Wnull-dereference -fsanitize=address -fsanitize=undefined -Wfloat-equal -Wconversion -Wfatal-errors -Woverflow -pedantic -Wextra")

set(CMAKE_CXX_FLAGS_RELEASE "-Wdouble-promotion -Wundef -Wall  -Wshadow -Wnull-dereference -Wfloat-equal -Wconversion -Wfatal-errors -Woverflow -pedantic -Wextra")

add_executable(${PROJECT_NAME}
         main.cpp
         node.cpp
         pushButton.cpp
         scene.cpp
         helpDialog.cpp
         resources.qrc
)

find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS StateMachine REQUIRED)


target_link_libraries(${PROJECT_NAME}
         PRIVATE Qt6::Gui
         PRIVATE Qt6::Core
         PRIVATE Qt6::Widgets
         PRIVATE Qt6::StateMachine
)