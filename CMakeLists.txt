cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(foosballmotortest C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


pico_sdk_init()

add_executable(foosballmotortest
        main.cpp communication.h error_checker.h pid_settings.h linear_movement/linear_movement.h linear_movement/encoder.h constants.h rotation/rotation.h system/system.h secondCoreEntry.h
        tusb_config.h
        usb_descriptors.c)


#target_compile_definitions(foosballmotortest PUBLIC USBD_VID=0x1444 USBD_PID=0x0001 USBD_MANUFACTURER="Fussball" USBD_PRODUCT="AI Tischkicker")
target_include_directories(foosballmotortest PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(foosballmotortest pico_stdlib hardware_pwm pico_multicore pico_unique_id tinyusb_device tinyusb_board tinyusb_bsp)

pico_enable_stdio_usb(foosballmotortest 0)
pico_enable_stdio_uart(foosballmotortest 1)

pico_add_extra_outputs(foosballmotortest)
