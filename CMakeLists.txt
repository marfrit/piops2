# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(piops2 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME} 
    piops2.c
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/piops2.pio)

pico_add_extra_outputs(${PROJECT_NAME})

# target_sources(${PROJECT_NAME} PRIVATE piops2.c)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pio)



pico_enable_stdio_uart(${PROJECT_NAME} 1)