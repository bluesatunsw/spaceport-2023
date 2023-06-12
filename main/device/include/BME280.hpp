// BME280.hpp
// Header file for the BME280 pressure sensor
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef BME280_H
#define BME280_H

#include "Device.hpp"
#include <stdint.h>
#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

/**
   some :IMPORTANT: notes :)


Sleep mode: no operation, all registers accessible, lowest power, selected after startup
Forced mode: perform one measurement, store results and return to sleep mode
Normal mode: perpetual cycling of measurements and inactive periods.
*/

#define BME_I2C_ADDR _u(0x76)

// hardware registers
#define REG_CONFIG _u(0xF5)
#define REG_CTRL_MEAS _u(0xF4)
#define REG_RESET _u(0xE0)

#define REG_TEMP_XLSB _u(0xFC)
#define REG_TEMP_LSB _u(0xFB)
#define REG_TEMP_MSB _u(0xFA)

#define REG_PRESSURE_XLSB _u(0xF9)
#define REG_PRESSURE_LSB _u(0xF8)
#define REG_PRESSURE_MSB _u(0xF7)

#define REG_WHOAMI      _u(0xD0)
// calibration registers
#define REG_DIG_T1_LSB _u(0x88)
#define REG_DIG_T1_MSB _u(0x89)
#define REG_DIG_T2_LSB _u(0x8A)
#define REG_DIG_T2_MSB _u(0x8B)
#define REG_DIG_T3_LSB _u(0x8C)
#define REG_DIG_T3_MSB _u(0x8D)
#define REG_DIG_P1_LSB _u(0x8E)
#define REG_DIG_P1_MSB _u(0x8F)
#define REG_DIG_P2_LSB _u(0x90)
#define REG_DIG_P2_MSB _u(0x91)
#define REG_DIG_P3_LSB _u(0x92)
#define REG_DIG_P3_MSB _u(0x93)
#define REG_DIG_P4_LSB _u(0x94)
#define REG_DIG_P4_MSB _u(0x95)
#define REG_DIG_P5_LSB _u(0x96)
#define REG_DIG_P5_MSB _u(0x97)
#define REG_DIG_P6_LSB _u(0x98)
#define REG_DIG_P6_MSB _u(0x99)
#define REG_DIG_P7_LSB _u(0x9A)
#define REG_DIG_P7_MSB _u(0x9B)
#define REG_DIG_P8_LSB _u(0x9C)
#define REG_DIG_P8_MSB _u(0x9D)
#define REG_DIG_P9_LSB _u(0x9E)
#define REG_DIG_P9_MSB _u(0x9F)

// number of calibration registers to be read
#define NUM_CALIB_PARAMS 24

struct bme280_calib_param
{
    // temperature params
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;

    // pressure params
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
};

class BME280 : public Device
{
public:
    BME280();

    // Device methods
    std::vector<baro_reading_t> read();
    status checkOK() override;
    status init(bool i2c_bus);

private:
    bme280_calib_param _calib_params;
    i2c_inst_t *i2cbus;

    // RP2040 example function prototypes
    void bme280_read_raw(int32_t* temp, int32_t* pressure);
    void bme280_reset();
    int32_t bme280_convert(int32_t temp, struct bme280_calib_param* params);
    int32_t bme280_convert_temp(int32_t temp, struct bme280_calib_param* params);
    int32_t bme280_convert_pressure(int32_t pressure, int32_t temp, struct bme280_calib_param* params);
    status bme280_get_calib_params(struct bme280_calib_param* params);
    
};

#endif
