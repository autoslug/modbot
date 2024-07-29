# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(swerve-pico C CXX ASM)

# enable error handling
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(swerve-pico swerve-pico.cpp
${CMAKE_CURRENT_LIST_DIR}/motor-library/motor-library.cpp
${CMAKE_CURRENT_LIST_DIR}/control-library/control-library.cpp
${CMAKE_CURRENT_LIST_DIR}/sensor-library/sensor-library.cpp
${CMAKE_CURRENT_LIST_DIR}/messaging-library/messaging-library.cpp
)


pico_set_program_name(swerve-pico "swerve-pico")
pico_set_program_version(swerve-pico "0.1")

pico_enable_stdio_uart(swerve-pico 0)
pico_enable_stdio_usb(swerve-pico 1)

# Add the standard library to the build
target_link_libraries(swerve-pico
        pico_stdlib
        pico_i2c_slave
        hardware_i2c
        hardware_pio
)

# Add the standard include files to the build
target_include_directories(swerve-pico PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(swerve-pico
  hardware_pwm
  hardware_pio
)

# # add all subdirectories of project
 target_include_directories(swerve-pico PRIVATE
 ${CMAKE_CURRENT_LIST_DIR}/control-library
 ${CMAKE_CURRENT_LIST_DIR}/messaging-library
 ${CMAKE_CURRENT_LIST_DIR}/motor-library
 ${CMAKE_CURRENT_LIST_DIR}/sensor-library
)

# link pio
pico_generate_pio_header(swerve-pico ${CMAKE_CURRENT_LIST_DIR}/motor-library/quadrature_encoder.pio)

pico_add_extra_outputs(swerve-pico)
