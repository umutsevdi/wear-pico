file(GLOB SW_UTIL_RES src/*.c )

include_directories(../)
add_library(sw_common ${SW_UTIL_RES})

target_link_libraries(sw_common
    pico_stdlib
    pico_time
    pico_multicore
    hardware_adc
    hardware_exception
    pico_stdlib
    hardware_spi
    hardware_i2c
)
