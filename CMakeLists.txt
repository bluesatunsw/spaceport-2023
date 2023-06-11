cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(obc_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
file(GLOB_RECURSE DEVICE_SRC "main/device/*.cpp")

add_executable(obc
    main/main.cpp
    main/System.cpp
    ${DEVICE_SRC}
)

target_include_directories(obc
    PRIVATE
    ${PROJECT_SOURCE_DIR}/main
    ${PROJECT_SOURCE_DIR}/main/device/include
    ${PROJECT_SOURCE_DIR}/main/include
)

pico_enable_stdio_usb(obc 1)
pico_enable_stdio_uart(obc 1)
pico_add_extra_outputs(obc)
target_link_libraries(obc pico_stdlib)