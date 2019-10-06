# CMakeList.txt : CMake project for VorbisPlugin, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(VorbisPlugin)

set (OGG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/ogg)
set (VORBIS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../dependency/vorbis)
set (OGG_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/ogg_build/lib)

add_subdirectory(${OGG_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/ogg_build)
add_subdirectory(${VORBIS_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build)

include_directories (VorbisPlugin 
    $<BUILD_INTERFACE:${OGG_ROOT}/include>
    $<BUILD_INTERFACE:${VORBIS_ROOT}>/include)

# Add source to this project's executable.
add_library (VorbisPlugin SHARED
    #../../../src/encoder.c
    #../../../src/PluginTest.c
    ../../../src/VorbisPluginEncoder.c
    ../../../src/VorbisPluginDecoder.c
)

target_link_libraries(VorbisPlugin ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build/lib/vorbis.lib)
target_link_libraries(VorbisPlugin ${CMAKE_CURRENT_BINARY_DIR}/ogg_build/ogg.lib)


# TODO: Add tests and install targets if needed.
