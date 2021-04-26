cmake_minimum_required (VERSION 3.14)

project (Tilze LANGUAGES CXX)

find_package (gempyre REQUIRED)
include(addResource)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    )

add_executable(${PROJECT_NAME}
    src/main.cpp
    include/animator.h
    gui/ui.html
    )

addResource(PROJECT ${PROJECT_NAME} TARGET include/ui_resource.h SOURCES gui/ui.html gui/favicon.ico)
	
target_link_libraries (${PROJECT_NAME} gempyre::gempyre)
