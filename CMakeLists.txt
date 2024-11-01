cmake_minimum_required(VERSION 3.12)
# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD "pico_w")

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Include Doxygen
find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/docs)
    message("Creating documentation using ${DOXYFILE_IN} ${DOXYFILE_OUT}")
    file(MAKE_DIRECTORY ${DOXYGEN_OUT})
    # Configure Doxygen output directory
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_OUTPUT_DIRECTORY ${DOXYGEN_OUT})
    # Add a custom target to generate documentation
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})
# Initialize the SDK
pico_sdk_init()

add_subdirectory(src)

add_compile_options(-Wall
        -O3
        -Wextra
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()
