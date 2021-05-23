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
    include/view.h
    include/cube.h
    include/table.h
    include/tilze.h
    include/drawable.h
    include/sparseiterator.h
    include/game.h
    include/gameenv.h
    gui/ui.html
    src/tilze.cpp
    src/cube.cpp
    src/animator.cpp
    src/gameenv.cpp
    src/game.cpp
    include/autoplay.h
    src/autoplay.cpp
    src/view.cpp
    )

add_compile_definitions(StripeCount=5)
add_compile_definitions(RowCount=5)

addResource(PROJECT ${PROJECT_NAME} TARGET include/ui_resource.h SOURCES gui/ui.html gui/favicon.ico)
	
target_link_libraries (${PROJECT_NAME} gempyre::gempyre)
