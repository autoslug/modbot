cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(pio_rotary_encoder VERSION 1.0.0)

add_executable(pio_rotary_encoder pio_rotary_encoder.cpp)

pico_sdk_init()

target_sources(pio_rotary_encoder PRIVATE pio_rotary_encoder.cpp)

target_link_libraries(pio_rotary_encoder PRIVATE
        pico_stdlib
        hardware_pio
        multicore
        pico_multicore
        )

pico_generate_pio_header(pio_rotary_encoder ${CMAKE_CURRENT_LIST_DIR}/pio_rotary_encoder.pio)

pico_add_extra_outputs(pio_rotary_encoder)

# enable usb output, disable uart output
pico_enable_stdio_usb(pio_rotary_encoder 1)
pico_enable_stdio_uart(pio_rotary_encoder 0)


