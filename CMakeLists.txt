cmake_minimum_required(VERSION 2.8)

#find_package(RtMidi)

## Use the variable PROJECT_NAME for changing the target name
set( PROJECT_NAME "MidiKey" )

## Set our project name
project(${PROJECT_NAME})

## Use all the *.cpp files we found under this folder for the project
FILE(GLOB SRCS "*.cpp" "*.c")

## Define the executable
add_executable(${PROJECT_NAME} ${SRCS})

target_link_directories(${PROJECT_NAME} PRIVATE 
"/usr/lib/x86_64-linux-gnu"
)

target_include_directories(${PROJECT_NAME} PRIVATE 
"/usr/include/rtmidi"
"/usr/include"
"/usr/local/include"
)
target_link_libraries(${PROJECT_NAME} 
xdo
rtmidi
pthread
)