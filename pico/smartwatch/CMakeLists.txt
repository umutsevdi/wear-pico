file(GLOB SOURCES src/*.c)
include_directories(include/)
add_executable(smartwatch ${SOURCES})


# pull in common dependencies
target_link_libraries(smartwatch pico_stdlib
    pico_time
    pico_multicore
    pico_btstack_ble # Needed for ble
    pico_btstack_cyw43 # Needed for ble
    pico_cyw43_arch_none # Needed for pico_w
    hardware_adc # don't think it's needed (but haven't tested)
    hardware_exception
)

# enable usb output, disable uart output
pico_enable_stdio_usb(smartwatch 1)
pico_enable_stdio_uart(smartwatch 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(smartwatch)
