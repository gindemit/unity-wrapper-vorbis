# CMakeList.txt : CMake project for VorbisPlugin, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (OGG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/ogg/include/)
set (OGG_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/ogg/lib/)
set (VORBIS_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/ogg/include/)

include_directories (VorbisPlugin 
    $<BUILD_INTERFACE:${OGG_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${VORBIS_INCLUDE_DIRS}>)

# Add source to this project's executable.
add_executable (VorbisPlugin "../../../src/encoder.c")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/ogg/ ${CMAKE_CURRENT_BINARY_DIR}/ogg_build)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/vorbis/ ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build)

# TODO: Add tests and install targets if needed.
