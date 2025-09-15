# CMakeList.txt : CMake project for VorbisPlugin, include source and define
# project specific logic here.

cmake_minimum_required(VERSION 3.6)

project(VorbisPlugin C)

option(VORBIS_ANDROID_PAGE_SIZE_16K "Build Android libraries with 16KB page size" OFF)
option(VORBIS_WEB_ASSEMBLY "Build WebAssembly static libraries" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS FALSE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(OGG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../dependency/ogg)
set(VORBIS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../dependency/vorbis)

if (VORBIS_IOS)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/VorbisPluginBuild)
elseif (VORBIS_OSX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/VorbisPluginBuild)
endif()

add_subdirectory(${OGG_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/ogg_build)

# Allow the in-tree Vorbis build to reuse the ogg target we just added instead of
# searching the host system for an installed libogg.
set(OGG_INCLUDE_DIR  ${OGG_ROOT}/include CACHE PATH "" FORCE)
set(OGG_INCLUDE_DIRS ${OGG_ROOT}/include CACHE PATH "" FORCE)
set(OGG_LIBRARIES ogg CACHE STRING "" FORCE)

add_subdirectory(${VORBIS_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/vorbis_build)

set(VORBIS_PLUGIN_LIBRARY_SOURCES
    ../../src/VorbisPluginEncoder.c
    ../../src/VorbisPluginDecoder.c)

if (VORBIS_IOS OR VORBIS_WEB_ASSEMBLY)
    add_library(VorbisPlugin STATIC ${VORBIS_PLUGIN_LIBRARY_SOURCES})
else()
    add_library(VorbisPlugin SHARED ${VORBIS_PLUGIN_LIBRARY_SOURCES})
endif()

if (WIN32)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()

# Include directories for the plugin sources
include_directories($<BUILD_INTERFACE:${OGG_ROOT}/include>)
include_directories($<BUILD_INTERFACE:${VORBIS_ROOT}/include>)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/ogg_build/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/vorbis_build/include)

set(VORBIS_PLUGIN_DEPENDENCIES vorbisfile vorbis vorbisenc ogg)

if (MSVC)
    target_compile_options(VorbisPlugin
        PRIVATE
            /std:c11
            /W3
            /Zc:wchar_t
            /Zc:inline)
else()
    target_compile_options(VorbisPlugin
        PRIVATE
            -std=c11
            -Wall
            -Wextra
            -Wno-unused-parameter
            -fvisibility=hidden)
endif()

if (VORBIS_LINUX)
    target_link_libraries(VorbisPlugin PRIVATE m)
endif()

target_link_libraries(VorbisPlugin PRIVATE ${VORBIS_PLUGIN_DEPENDENCIES})

if (ANDROID AND VORBIS_ANDROID_PAGE_SIZE_16K)
    foreach(lib_target IN ITEMS VorbisPlugin ${VORBIS_PLUGIN_DEPENDENCIES})
        if (TARGET ${lib_target})
            target_link_options(${lib_target} PRIVATE "-Wl,-z,max-page-size=16384" "-Wl,-z,common-page-size=16384")
        endif()
    endforeach()
endif()

if (WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(VORBIS_PLUGIN_TARGET_ARCHITECTURE x86_64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(VORBIS_PLUGIN_TARGET_ARCHITECTURE x86)
    endif()
elseif(ANDROID)
    set(VORBIS_PLUGIN_TARGET_ARCHITECTURE ${CMAKE_ANDROID_ARCH_ABI})
else()
    set(VORBIS_PLUGIN_TARGET_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(VORBIS_PLUGIN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../out/$<CONFIG>/Plugins/$<PLATFORM_ID>/${VORBIS_PLUGIN_TARGET_ARCHITECTURE}")

add_custom_command(
    TARGET VorbisPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${VORBIS_PLUGIN_OUTPUT_DIR}")

add_custom_command(
    TARGET VorbisPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:VorbisPlugin>"
        "${VORBIS_PLUGIN_OUTPUT_DIR}/$<TARGET_FILE_NAME:VorbisPlugin>"
    COMMENT "Copied VorbisPlugin to out directory")

foreach(dep IN LISTS VORBIS_PLUGIN_DEPENDENCIES)
    if (TARGET ${dep})
        add_custom_command(
            TARGET VorbisPlugin POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${dep}>"
                "${VORBIS_PLUGIN_OUTPUT_DIR}/$<TARGET_FILE_NAME:${dep}>"
            COMMENT "Copied $<TARGET_FILE_NAME:${dep}> to out directory")
    endif()
endforeach()

if (VORBIS_WEB_ASSEMBLY)
    add_custom_command(
        TARGET VorbisPlugin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/../../out/Plugins/WebGL")
    foreach(dep IN ITEMS VorbisPlugin ${VORBIS_PLUGIN_DEPENDENCIES})
        if (TARGET ${dep})
            add_custom_command(
                TARGET VorbisPlugin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_FILE:${dep}>"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../out/Plugins/WebGL/$<TARGET_FILE_NAME:${dep}>"
                COMMENT "Copied $<TARGET_FILE_NAME:${dep}> to out/Plugins/WebGL")
        endif()
    endforeach()
endif()

if (ANDROID)
    add_custom_command(
        TARGET VorbisPlugin POST_BUILD
        COMMAND "${ANDROID_TOOLCHAIN_PREFIX}strip" -g -S -d --strip-debug --verbose
                "${VORBIS_PLUGIN_OUTPUT_DIR}/$<TARGET_FILE_NAME:VorbisPlugin>"
        COMMENT "Strip debug symbols done on final binary.")
endif()

if (NOT VORBIS_WEB_ASSEMBLY)
    add_executable(VorbisPluginTest ../../src/PluginTest.c)
    target_link_libraries(VorbisPluginTest VorbisPlugin)
    if (VORBIS_IOS)
        set_xcode_property(VorbisPluginTest PRODUCT_BUNDLE_IDENTIFIER "com.soulside.app" All)
    endif()
endif()
