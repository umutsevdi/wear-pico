file(GLOB WP_DEV_RES src/*.c )

include_directories(..)
add_library(wp_dev ${WP_DEV_RES})

target_link_libraries(wp_dev
    pico_stdlib
    pico_time
    pico_multicore
    hardware_adc
    hardware_exception
    pico_stdlib
    hardware_spi
    hardware_i2c
)
