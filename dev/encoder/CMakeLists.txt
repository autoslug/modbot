cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(quadrature_encoder VERSION 1.0.0)

add_executable(quadrature_encoder quadrature_encoder.cpp)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

target_sources(quadrature_encoder PRIVATE quadrature_encoder.cpp)

target_link_libraries(quadrature_encoder PRIVATE
        pico_stdlib
        hardware_pio
        )

pico_generate_pio_header(quadrature_encoder ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)

pico_add_extra_outputs(quadrature_encoder)

# enable usb output, disable uart output
pico_enable_stdio_usb(quadrature_encoder 1)
pico_enable_stdio_uart(quadrature_encoder 0)


