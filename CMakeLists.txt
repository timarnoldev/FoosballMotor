cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(foosballmotortest C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


pico_sdk_init()

add_executable(foosballmotortest
        main.cpp communication.h error_checker.h pid_settings.h linear_movement/linear_movement.h linear_movement/encoder.h constants.h rotation/rotation.h system/system.h secondCoreEntry.h)

target_link_libraries(foosballmotortest pico_stdlib hardware_pwm pico_multicore)

pico_enable_stdio_usb(foosballmotortest 1)
pico_enable_stdio_uart(foosballmotortest 1)

pico_add_extra_outputs(foosballmotortest)
