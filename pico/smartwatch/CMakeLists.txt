file(GLOB SOURCES src/*.c)
file(GLOB INCLUDE include/*.h)

add_executable(smartwatch ${SOURCES})
target_link_libraries(smartwatch pico_stdlib)

# pull in common dependencies
target_link_libraries(smartwatch pico_stdlib pico_time pico_multicore)

# enable usb output, disable uart output
pico_enable_stdio_usb(smartwatch 1)
pico_enable_stdio_uart(smartwatch 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(smartwatch)
