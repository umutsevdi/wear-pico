file(GLOB WP_UTIL_RES src/*.c )

include_directories(../)
add_library(wp_utils ${WP_UTIL_RES})

target_link_libraries(wp_utils
    wp_dev
    pico_stdlib
    pico_time
)
