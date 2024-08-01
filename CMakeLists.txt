cmake_minimum_required(VERSION 3.10)
project(FLAP VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set OpenGL Policy
set(OpenGL_GL_PREFERENCE LEGACY)

# Include directories imported libraries
include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/glad)
include_directories(${PROJECT_SOURCE_DIR}/inc/imgui)
include_directories(${PROJECT_SOURCE_DIR}/inc/imgui/backends)
include_directories(${PROJECT_SOURCE_DIR}/inc/imgui-knobs)
include_directories(${PROJECT_SOURCE_DIR}/inc/miniaudio)
include_directories(${PROJECT_SOURCE_DIR}/inc/rtmidi)

# Include GLFW source and headers
add_subdirectory(${PROJECT_SOURCE_DIR}/inc/glfw ${PROJECT_BINARY_DIR}/glfw)

# This line includes the GLFW headers from the correct path
include_directories(${PROJECT_BINARY_DIR}/glfw/include ${PROJECT_SOURCE_DIR}/inc/glfw/include)

# Include all source files from the 'src' directory and specific files
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(APPEND SOURCES ${PROJECT_SOURCE_DIR}/inc/rtmidi/RtMidi.cpp)
list(APPEND SOURCES
    ${PROJECT_SOURCE_DIR}/inc/glad/glad.c
    ${PROJECT_SOURCE_DIR}/inc/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui/backends/imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/inc/imgui-knobs/imgui-knobs.cpp
)

# Create an object library for shared sources
add_library(shared_sources OBJECT ${SOURCES})

# Add the executables and link the shared object library
add_executable(flapTest ${PROJECT_SOURCE_DIR}/test/test.cpp)

# Link shared sources and other libraries
target_link_libraries(flapTest PRIVATE $<TARGET_OBJECTS:shared_sources> glfw ${CMAKE_DL_LIBS})

# Platform-specific settings and dependencies
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Threads REQUIRED)
    target_compile_definitions(shared_sources PRIVATE __LINUX_ALSA__)
    target_link_libraries(flapTest PRIVATE asound Threads::Threads m)
    # Link against GL and GLFW for ImGui
    find_package(OpenGL REQUIRED)
    target_link_libraries(flapTest PRIVATE ${OPENGL_LIBRARIES})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(shared_sources PRIVATE __MACOSX_CORE__)
    find_library(COREMIDI CoreMIDI)
    find_library(COREAUDIO CoreAudio)
    find_library(COREFOUNDATION CoreFoundation)
    target_link_libraries(flapTest PRIVATE ${COREMIDI} ${COREAUDIO} ${COREFOUNDATION} m)
    # Link against OpenGL and GLFW for ImGui
    find_package(OpenGL REQUIRED)
    target_link_libraries(flapTest PRIVATE ${OPENGL_LIBRARIES})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(shared_sources PRIVATE __WINDOWS_MM__)
    target_link_libraries(flapTest PRIVATE winmm)
    # Link against OpenGL and GLFW for ImGui
    find_package(OpenGL REQUIRED)
    target_link_libraries(flapTest PRIVATE ${OPENGL_LIBRARIES})
endif()

# SIMD optimizations for GCC and Clang on x86_64 architecture
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    target_compile_options(shared_sources PRIVATE -msse2 -mavx2)
  endif()
endif()