cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(obc_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()


# # Add the path to the ICM-20948 library
# set(ICM20948_PATH ${CMAKE_CURRENT_SOURCE_DIR}/icm20948)

# # Specify the path to the ICM-20948 static library
# set(ICM20948_LIB ${ICM20948_PATH}/lib/lib_icm20948.a)

set(LITTLEFS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/littlefs)
set(H3LIS100DLTR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/h3lis100dl-pid)

file(GLOB_RECURSE DEVICE_SRC "main/device/*.cpp")


add_executable(obc
    main/main.cpp
    main/System.cpp
    main/fs.cpp
    ${LITTLEFS_SRC}
    ${LITTLEFS_PATH}/lfs.c
    ${LITTLEFS_PATH}/lfs_util.c
    ${H3LIS100DLTR_PATH}/h3lis100dl_reg.c
    ${DEVICE_SRC}
)

target_include_directories(obc
    PRIVATE
    ${PROJECT_SOURCE_DIR}/main
    ${PROJECT_SOURCE_DIR}/main/device/include
    ${PROJECT_SOURCE_DIR}/main/include
    ${H3LIS100DLTR_PATH}
    # ${ICM20948_PATH}/inc  # Include the path to the ICM-20948 header files
    ${LITTLEFS_PATH}/
)

# pico_enable_stdio_uart(obc 1)
pico_enable_stdio_usb(obc 1)
pico_add_extra_outputs(obc)

# Link the ICM-20948 and LittleFS libraries
target_link_libraries(obc pico_stdlib hardware_i2c hardware_spi hardware_flash)