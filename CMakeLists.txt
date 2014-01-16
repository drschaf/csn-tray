include(UsePkgConfig)

cmake_minimum_required(VERSION 2.6)
project(CSNTRAY)

add_executable(csn-tray main.c src/conf.c src/tray.c src/csn_api.c)

find_package(GTK2)
find_package(CURL)

PKGCONFIG(gtk+-2.0 GTK2_INCLUDE_DIR GTK2_LINK_DIR GTK2_LINK_FLAGS
                   GTK2_CFLAGS)

set(CMAKE_C_FLAGS "${GTK2_DEFINES}")

target_link_libraries(csn-tray curl "${GTK2_LIBRARIES}" "${GTK2_LINK_FLAGS}")
include_directories("${GTK2_INCLUDE_DIRS}")
