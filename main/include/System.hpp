// System.hpp
// System manager header files.
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef SYSTEM_H
#define SYSTEM_H

// Standard dependencies
#include <stdint.h>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <memory>
#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "hardware/spi.h"

// Our dependencies
#include "types.hpp"
#include "DS3231.hpp"
#include "H3LIS100DLTR.hpp"
#include "BME280.hpp"
#include "ICM20948.hpp"
#include "Payload.hpp"
#include "fs.hpp"

// ### Pins for system control ###

// #define PIN_OFFLOAD (gpio_num_t) 2
// #define PIN_TESTMODE (gpio_num_t) 3

// // TODO: check these
// #define PIN_SCL idf::SCL_GPIO(22)
// #define PIN_SDA idf::SDA_GPIO(21)

// ### config ###
#define I2C_ENABLE
// #define SPI_ENABLE


// ### enums ###
enum system_mode {
    MODE_NORMAL,
    MODE_OFFLOAD,
    MODE_TEST,
    MODE_DIAGNOSTIC,
};

enum stage {
    STAGE_PAD,
    STAGE_POWERED_ASCENT,
    STAGE_COAST,
    STAGE_EXPERIMENT,
    STAGE_TERMINATION,
};


// ### Constants ###
#define ARM_DELAY_SECONDS 10 * 1
#define POWERED_ASCENT_DURATION_MS 65 * 100 // 6.5 seconds
#define BURNOUT_PAYLOAD_DELAY_MS 10 * 1000 
#define TERMINATION_DELAY_MS 10 * 60 * 1000; 
#define PIN_SCL0 1
#define PIN_SDA0 0
#define PIN_CS 15 // Placeholder
#define PIN_SCK 14 // Placeholder
#define PIN_MISO 12 // Placeholder
#define PIN_MOSI 13 // Placeholder


// ### Class prototype ### 
class System {
public:
    system_mode mode = MODE_NORMAL;
    stage flight_stage = STAGE_PAD;
    // Default constructor
    System();
    void init(void);
    // Readings
    std::vector<accel_reading_t> accelread(void);
    std::vector<imu_reading_t> imuread(void);
    std::vector<baro_reading_t> baroread(void);
    rtc_reading_t rtcread(void);

    // ioctl
    int flash_flush(void);
    void log_init(void);
    void log_msg(std::string msg, log_type type);
    void offload(void);
    void sensor_init(void);

    // Staging
    void await_arm(void);
    void await_launch(void);

    Payload payload;
private:
    // Private variables
    flash_mode flashmode;


    // Devices
    DS3231 rtc;
    H3LIS100DLTR acc0;
    // H3LIS100DLTR acc1;
    BME280 baro0;
    // BME280 baro1;
    // ICM20948 imu0;
    // ICM20948 imu1;
    // std::shared_ptr<idf::I2CMaster> i2c;

    // Private methods
    void log_internal(std::string msg, log_type type);

    // Startup checks
    bool check_uart(void);
    bool check_power(void);
    bool check_payload(void);

    // Callback functions


    // Interrupt handler for I2C devices
    static void interrupt_handler(void *param);
};
static int64_t staging_callback(alarm_id_t id, void *user_data);

#endif
