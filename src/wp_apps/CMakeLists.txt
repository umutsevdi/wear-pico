file(GLOB WP_APP_RES src/*.c )

include_directories(../waveshare/lib/Config)
include_directories(../waveshare/lib/LCD)
include_directories(../waveshare/lib/Fonts)
include_directories(../waveshare/lib/GUI)
include_directories(../)

add_library(wp_apps ${WP_APP_RES})
target_link_libraries(wp_apps
    wp_res
    wp_utils
    wp_dev
    pico_stdlib
    pico_time
    pico_multicore
    #    pico_btstack_ble # Needed for ble
    #    pico_btstack_cyw43 # Needed for ble
    #   pico_cyw43_arch_none # Needed for pico_w
    hardware_adc # don't think it's needed (but haven't tested)
    hardware_exception
    # waveshare_begin
    LCD GUI Fonts Config pico_stdlib hardware_spi hardware_i2c
    # waveshare_ends
)
