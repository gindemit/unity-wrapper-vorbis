# CMakeList.txt : CMake project for VorbisPlugin, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.6)

project(VorbisPlugin)

set (OGG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../dependency/ogg)
set (VORBIS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../dependency/vorbis)
set (OGG_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/ogg_build/lib)
if (VORBIS_IOS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/VorbisPluginBuild)
elseif (VORBIS_OSX)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/VorbisPluginBuild)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/ogg_build/include)

add_subdirectory(${OGG_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/ogg_build)
add_subdirectory(${VORBIS_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build)

include_directories ($<BUILD_INTERFACE:${OGG_ROOT}/include>)
include_directories ($<BUILD_INTERFACE:${VORBIS_ROOT}/include>)

set (VORBIS_PLUGIN_LIBRARY_SOURCES 
    ../../src/VorbisPluginEncoder.c
    ../../src/VorbisPluginDecoder.c)
    
if (VORBIS_IOS)
  add_library (VorbisPlugin STATIC ${VORBIS_PLUGIN_LIBRARY_SOURCES})
else()
  add_library (VorbisPlugin SHARED ${VORBIS_PLUGIN_LIBRARY_SOURCES})
endif()

if (WIN32)
  set(OGG_LIBRARY_NAME ogg.lib)
  set(VORBIS_LIBRARY_NAME vorbis.lib)
  set(VORBIS_FILE_LIBRARY_NAME vorbisfile.lib)
elseif(VORBIS_OSX)
  set(OGG_LIBRARY_NAME Release/libogg.a)
  set(VORBIS_LIBRARY_NAME Release/libvorbis.a)
  set(VORBIS_FILE_LIBRARY_NAME Release/libvorbisfile.a)
  #set_target_properties(VorbisPlugin PROPERTIES BUNDLE TRUE)
  #set_target_properties(VorbisPlugin PROPERTIES SUFFIX ".bundle")
else()
  set(OGG_LIBRARY_NAME libogg.a)
  set(VORBIS_LIBRARY_NAME libvorbis.a)
  set(VORBIS_FILE_LIBRARY_NAME libvorbisfile.a)
endif()

target_link_libraries(VorbisPlugin ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build/lib/${VORBIS_FILE_LIBRARY_NAME})
target_link_libraries(VorbisPlugin ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build/lib/${VORBIS_LIBRARY_NAME})
target_link_libraries(VorbisPlugin ${CMAKE_CURRENT_BINARY_DIR}/ogg_build/${OGG_LIBRARY_NAME})

add_dependencies(vorbisfile vorbis ogg)
add_dependencies(vorbis ogg)
add_dependencies(VorbisPlugin vorbisfile vorbis ogg)


#Adding simple test program:
add_executable(VorbisPluginTest ../../src/PluginTest.c)
target_link_libraries(VorbisPluginTest VorbisPlugin)
if (VORBIS_IOS)
  set_xcode_property(VorbisPluginTest PRODUCT_BUNDLE_IDENTIFIER "com.soulside.app" All)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (VORBIS_PLUGIN_TARGET_ARCHITECTURE x86_64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set (VORBIS_PLUGIN_TARGET_ARCHITECTURE x86)
endif()
add_custom_command(
    TARGET VorbisPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../out/$<CONFIG>/Plugins/$<PLATFORM_ID>/${VORBIS_PLUGIN_TARGET_ARCHITECTURE}")

add_custom_command(
    TARGET VorbisPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:VorbisPlugin>"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../out/$<CONFIG>/Plugins/$<PLATFORM_ID>/${VORBIS_PLUGIN_TARGET_ARCHITECTURE}/$<TARGET_FILE_NAME:VorbisPlugin>"
    COMMENT "Copied final library to out directory")

# Uncomment following lines if you want to copy the pdb files for Debug builds:
if (WIN32)
add_custom_command(
    TARGET VorbisPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_PDB_FILE:VorbisPlugin>"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../out/$<CONFIG>/Plugins/$<PLATFORM_ID>/${VORBIS_PLUGIN_TARGET_ARCHITECTURE}/$<TARGET_PDB_FILE_NAME:VorbisPlugin>")
endif()
# uncomment end
if (ANDROID)
add_custom_command(
    TARGET VorbisPlugin POST_BUILD
    COMMAND "${ANDROID_TOOLCHAIN_PREFIX}strip" -g -S -d --strip-debug --verbose
            "${CMAKE_CURRENT_SOURCE_DIR}/../../out/$<CONFIG>/Plugins/$<PLATFORM_ID>/${VORBIS_PLUGIN_TARGET_ARCHITECTURE}/$<TARGET_FILE_NAME:VorbisPlugin>"
    COMMENT "Strip debug symbols done on final binary.")
endif()