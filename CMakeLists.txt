cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(obc_project C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

file(GLOB_RECURSE DEVICE_SRC "main/device/*.cpp")

# Add the path to the ICM-20948 library
set(ICM20948_PATH ${CMAKE_CURRENT_SOURCE_DIR}/icm20948)

# Specify the path to the ICM-20948 static library
set(ICM20948_LIB ${ICM20948_PATH}/lib/lib_icm20948.a)

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
    ${ICM20948_PATH}/inc  # Include the path to the ICM-20948 header files
)

pico_enable_stdio_usb(obc 1)
pico_enable_stdio_uart(obc 1)
pico_add_extra_outputs(obc)

# Link the ICM-20948 static library
target_link_libraries(obc pico_stdlib hardware_i2c hardware_spi ${ICM20948_LIB})
