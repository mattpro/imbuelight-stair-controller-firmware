cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)
#include(/mnt/d/LocalRepo/ImbueLight/imbuelight-stair-controller/firmware/pico-sdk/tools/CMakeLists.txt)

project(imbuelight_stair_controller C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD "pico_w")

pico_sdk_init()

include_directories(src/rtt/Config src/rtt/RTT)

include_directories(src/rtt/Config) 

add_executable(${PROJECT_NAME}
    src/main.c  
    src/pwm.c
    src/oled/ssd1306_fonts.c
    src/oled/ssd1306.c
    src/pwm_pio.c
    src/my_i2c.c
    src/io_exp.c
    src/utils.c
    src/settings.c
    src/ble_communication.c
    src/effects.c
    src/additional_fun.c
    #src/rtt/RTT/SEGGER_RTT.c 
    #src/rtt/RTT/SEGGER_RTT_printf.c
    src/server_common.c
    src/my_rtc.c
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_pio
    hardware_pwm
    hardware_i2c  
    hardware_flash  
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none  
    hardware_rtc
)

target_include_directories(${PROJECT_NAME} PRIVATE
     ${CMAKE_CURRENT_LIST_DIR}/src # For btstack config
     )
pico_btstack_make_gatt_header(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_LIST_DIR}/src/temp_sensor.gatt"
    )
 

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
pico_generate_pio_header(${PROJECT_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/src/pwm.pio)





