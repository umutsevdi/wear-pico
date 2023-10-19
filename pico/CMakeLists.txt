cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
project(smartwatch)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

file(GLOB SOURCES src/*.c)
file(GLOB INCLUDE include/*.h)

add_executable(smartwatch ${SOURCES})
target_link_libraries(smartwatch pico_stdlib)
pico_add_extra_outputs(smartwatch)
