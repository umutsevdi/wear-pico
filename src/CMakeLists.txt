file(GLOB SOURCES *.c)
add_executable(wear-pico ${SOURCES})

# Waveshare dependencies
#添加编译子目录
add_subdirectory(waveshare/lib/Config)
add_subdirectory(waveshare/lib/LCD)
add_subdirectory(waveshare/lib/Fonts)
add_subdirectory(waveshare/lib/GUI)
# add_subdirectory(bluetooth-stdio)
add_subdirectory(wp_res/)
add_subdirectory(wp_common/)
add_subdirectory(wp_dev/)
add_subdirectory(wp_apps/)
add_subdirectory(wp_bt/)

#添加头文件目录
include_directories(waveshare/lib/Config)
include_directories(waveshare/lib/Fonts)
include_directories(waveshare/lib/GUI)
include_directories(waveshare/lib/LCD)
include_directories(./)

# pull in common dependencies
target_link_libraries(wear-pico
    wp_res
    wp_utils
    wp_dev
    wp_apps
    wp_bt

    # bluetooth_stdio

    pico_stdlib
    pico_time

    pico_multicore
    hardware_adc
    hardware_exception
    # waveshare_begin
    LCD GUI Fonts Config hardware_spi hardware_i2c
    # waveshare_ends
)

# enable usb output, disable uart output
pico_enable_stdio_usb(wear-pico 1)
pico_enable_stdio_uart(wear-pico 0)

# pico_btstack_make_gatt_header(wear-pico PRIVATE "${CMAKE_CURRENT_LIST_DIR}/sw.gatt")
# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(wear-pico)

