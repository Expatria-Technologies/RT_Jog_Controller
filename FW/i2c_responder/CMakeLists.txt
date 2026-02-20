#SET(GCC_COVERAGE_COMPILE_FLAGS "-L")
add_library(pico_neopixel INTERFACE)

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(app_main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
pico_sdk_init()
add_subdirectory(i2c_slave)
add_executable(app_main
BitBang_I2C.cpp
BitBang_I2C.h
ss_oled.cpp
ss_oled.h
Adafruit_NeoPixel.cpp
Adafruit_NeoPixel.hpp
app_main.cpp)
pico_generate_pio_header(app_main  ${CMAKE_CURRENT_LIST_DIR}/ws2812byte.pio)
#target_sources(i2c_slave PRIVATE)
pico_enable_stdio_usb(app_main 1)
pico_enable_stdio_uart(app_main 1)
pico_add_extra_outputs(app_main)

if(DEFINED BUILD_SHA)
    target_compile_definitions(app_main PRIVATE BUILD_SHA="${BUILD_SHA}")
    set_target_properties(app_main PROPERTIES OUTPUT_NAME "jog2k-${BUILD_SHA}")
else()
    set_target_properties(app_main PROPERTIES OUTPUT_NAME "jog2k")
endif()

# Pull in pico libraries that we need
# target_link_libraries(pico_neopixel INTERFACE pico_stdlib hardware_pio pico_malloc pico_mem_ops)
target_link_libraries(app_main i2c_slave pico_stdlib hardware_i2c pico_stdlib hardware_pio pico_malloc pico_mem_ops)
#target_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)


