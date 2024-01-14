file(GLOB SOURCES *.c)
add_executable(smartwatch ${SOURCES})

# Waveshare dependencies
#添加编译子目录
add_subdirectory(waveshare/lib/Config)
add_subdirectory(waveshare/lib/LCD)
add_subdirectory(waveshare/lib/Fonts)
add_subdirectory(waveshare/lib/GUI)
# add_subdirectory(bluetooth-stdio)
add_subdirectory(sw_res/)
add_subdirectory(sw_common/)
add_subdirectory(sw_os/)
add_subdirectory(sw_apps/)
add_subdirectory(sw_bt/)

#添加头文件目录
include_directories(waveshare/lib/Config)
include_directories(waveshare/lib/Fonts)
include_directories(waveshare/lib/GUI)
include_directories(waveshare/lib/LCD)
include_directories(./)

# pull in common dependencies
target_link_libraries(smartwatch
    sw_res
    sw_utils
    sw_os
    sw_apps
    sw_bt

    # bluetooth_stdio

    pico_stdlib
    pico_time

    pico_multicore
    hardware_adc
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

