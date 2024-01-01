file(GLOB SW_UTIL_RES src/*.c )

include_directories(../)
add_library(sw_common ${SW_UTIL_RES})

target_link_libraries(sw_common
    pico_stdlib
    pico_time
)
