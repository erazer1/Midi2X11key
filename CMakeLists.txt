cmake_minimum_required(VERSION 3.8)

set (MIDIKEY_MAJOR_VERSION 0)
set (MIDIKEY_MINOR_VERSION 0)
set (MIDIKEY_PATCH_VERSION 1)

#find_package(RtMidi)

## Use the variable PROJECT_NAME for changing the target name
set( PROJECT_NAME "MidiKey" )

## Set our project name
project(${PROJECT_NAME})

set (${PROJECT_NAME}_SOURCES
main.cpp
)

configure_file(
   ${PROJECT_SOURCE_DIR}/version.h.in
   ${PROJECT_SOURCE_DIR}/version.h
   @ONLY)  

## Define the executable
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

# add dependencies
include(cmake/CPM.cmake)

CPMAddPackage("gh:nlohmann/json@3.11.2")
CPMAddPackage("gh:jarro2783/cxxopts@3.1.1")
# rtmidi does not use v prefix for this version
#CPMAddPackage("gh:thestk/rtmidi#5.0.0")

target_link_directories(${PROJECT_NAME} PRIVATE 
"/usr/lib/x86_64-linux-gnu"
)

target_include_directories(${PROJECT_NAME} PRIVATE 
"/usr/include/rtmidi"
"/usr/include"
#"/usr/local/include"
)

target_link_libraries(${PROJECT_NAME} 
nlohmann_json::nlohmann_json
cxxopts::cxxopts
rtmidi
xdo
pthread
)