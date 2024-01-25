file(GLOB WP_BT_RES src/*.c )

include_directories(../)
add_library(wp_bt ${WP_BT_RES})

target_link_libraries(wp_bt
    wp_utils
    pico_stdlib
    pico_time
)
