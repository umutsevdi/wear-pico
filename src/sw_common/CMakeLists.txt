file(GLOB SW_UTIL_RES src/*.c )

include_directories(../)
add_library(sw_utils ${SW_UTIL_RES})

target_link_libraries(sw_utils
    sw_os
    pico_stdlib
    pico_time
)
