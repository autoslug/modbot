# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.5)

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

project(basic_swerve_pico C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(basic_swerve_pico basic_swerve_pico.c )

pico_set_program_name(basic_swerve_pico "basic_swerve_pico")
pico_set_program_version(basic_swerve_pico "0.1")

pico_enable_stdio_uart(basic_swerve_pico 0)
pico_enable_stdio_usb(basic_swerve_pico 1)

# Add the standard library to the build
target_link_libraries(basic_swerve_pico
        pico_stdlib
        pico_i2c_slave
        hardware_i2c
        hardware_pwm
        )

# Add the standard include files to the build
target_include_directories(basic_swerve_pico PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(basic_swerve_pico 
        hardware_i2c
        )

pico_add_extra_outputs(basic_swerve_pico)

