file(GLOB SW_BT_RES src/*.c )

include_directories(../)
add_library(sw_bt ${SW_BT_RES})

target_link_libraries(sw_bt
    sw_utils
    pico_stdlib
    pico_time
)
