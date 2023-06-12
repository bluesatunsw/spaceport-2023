// types.hpp
// Common datastructures.
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <stdint.h>

typedef struct {
    uint16_t acc_x;
    uint16_t acc_y;
    uint16_t acc_z;
} accel_reading_t;

typedef struct {
    uint16_t acc_x;
    uint16_t acc_y;
    uint16_t acc_z;
    uint16_t gyr_x;
    uint16_t gyr_y;
    uint16_t gyr_z;
} imu_reading_t;

typedef uint32_t rtc_reading_t;

typedef struct {
    uint16_t temp;
    uint16_t pressure;
} baro_reading_t;

enum status {
    STATUS_OK,
    STATUS_MISBEHAVING,
    STATUS_FAILED,
};
enum flash_mode {
    FLASH_INTERNAL,
    FLASH_EXTERNAL
};

enum log_type {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL
};

#endif
