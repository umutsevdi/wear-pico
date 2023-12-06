file(GLOB SOURCES *.c)
add_executable(smartwatch ${SOURCES})

# Waveshare dependencies
#添加编译子目录
add_subdirectory(waveshare/lib/Config)
add_subdirectory(waveshare/lib/LCD)
add_subdirectory(waveshare/lib/Fonts)
add_subdirectory(waveshare/lib/GUI)
add_subdirectory(resources/)
add_subdirectory(sw_utils/)
add_subdirectory(ui/)

#添加头文件目录
include_directories(waveshare/lib/Config)
include_directories(waveshare/lib/Fonts)
include_directories(waveshare/lib/GUI)
include_directories(waveshare/lib/LCD)
include_directories(resources/include/)
include_directories(sw_utils/include/)
include_directories(ui/include/)

# pull in common dependencies
target_link_libraries(smartwatch
    sw_resources
    sw_utils
    sw_ui
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

# enable usb output, disable uart output
pico_enable_stdio_usb(smartwatch 1)
pico_enable_stdio_uart(smartwatch 0)

# pico_btstack_make_gatt_header(smartwatch PRIVATE "${CMAKE_CURRENT_LIST_DIR}/sw.gatt")
# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(smartwatch)

