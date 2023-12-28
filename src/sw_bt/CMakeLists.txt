# Standalone example that reads from the on board temperature sensor and sends notifications via BLE
# Flashes slowly each second to show it's running
file(GLOB SW_BT_RES src/*.c )
include_directories(../)
add_library(sw_bt ${SW_BT_RES})

target_link_libraries(sw_bt
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_adc
)
pico_btstack_make_gatt_header(sw_bt PRIVATE "${CMAKE_CURRENT_LIST_DIR}/sw_bt.gatt")
