// BME280.cpp
// Implementation of BME280 barometer. Shamelessly stolen from the RP2040 examples
// Matt Rossouw (omeh-a)
// 05/2023

#include "BME280.hpp"

void BME280::bme280_read_raw(int32_t* temp, int32_t* pressure) {
    // bme280 data registers are auto-incrementing and we have 3 temperature and
    // pressure registers each, so we start at 0xF7 and read 6 bytes to 0xFC
    // note: normal mode does not require further ctrl_meas and config register writes

    uint8_t buf[6];
    uint8_t reg = REG_PRESSURE_MSB;
    i2c_write_blocking(i2cbus, BME_I2C_ADDR, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2cbus, BME_I2C_ADDR, buf, 6, false);  // false - finished with bus

    // store the 20 bit read in a 32 bit signed integer for conversion
    *pressure = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
    *temp = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
}

void BME280::bme280_reset() {
    // reset the device with the power-on-reset procedure
    uint8_t buf[2] = { REG_RESET, 0xB6 };
    i2c_write_blocking(i2cbus, BME_I2C_ADDR, buf, 2, false);
}

// intermediate function that calculates the fine resolution temperature
// used for both pressure and temperature conversions
int32_t BME280::bme280_convert(int32_t temp, struct bme280_calib_param* params) {
    // use the 32-bit fixed point compensation implementation given in the
    // datasheet
    
    int32_t var1, var2;
    var1 = ((((temp >> 3) - ((int32_t)params->dig_t1 << 1))) * ((int32_t)params->dig_t2)) >> 11;
    var2 = (((((temp >> 4) - ((int32_t)params->dig_t1)) * ((temp >> 4) - ((int32_t)params->dig_t1))) >> 12) * ((int32_t)params->dig_t3)) >> 14;
    return var1 + var2;
}

int32_t BME280::bme280_convert_temp(int32_t temp, struct bme280_calib_param* params) {
    // uses the bme280 calibration parameters to compensate the temperature value read from its registers
    int32_t t_fine = bme280_convert(temp, params);
    return (t_fine * 5 + 128) >> 8;
}

int32_t BME280::bme280_convert_pressure(int32_t pressure, int32_t temp, struct bme280_calib_param* params) {
    // uses the bme280 calibration parameters to compensate the pressure value read from its registers

    int32_t t_fine = bme280_convert(temp, params);

    int32_t var1, var2;
    uint32_t converted = 0.0;
    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)params->dig_p6);
    var2 += ((var1 * ((int32_t)params->dig_p5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)params->dig_p4) << 16);
    var1 = (((params->dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)params->dig_p2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)params->dig_p1)) >> 15);
    if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }
    converted = (((uint32_t)(((int32_t)1048576) - pressure) - (var2 >> 12))) * 3125;
    if (converted < 0x80000000) {
        converted = (converted << 1) / ((uint32_t)var1);
    } else {
        converted = (converted / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)params->dig_p9) * ((int32_t)(((converted >> 3) * (converted >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(converted >> 2)) * ((int32_t)params->dig_p8)) >> 13;
    converted = (uint32_t)((int32_t)converted + ((var1 + var2 + params->dig_p7) >> 4));
    return converted;
}

status BME280::bme280_get_calib_params(struct bme280_calib_param* params) {
    // raw temp and pressure values need to be calibrated according to
    // parameters generated during the manufacturing of the sensor
    // there are 3 temperature params, and 9 pressure params, each with a LSB
    // and MSB register, so we read from 24 registers

    uint8_t buf[NUM_CALIB_PARAMS] = { 0 };
    uint8_t reg = REG_DIG_T1_LSB;
    int err = i2c_write_blocking(i2cbus, BME_I2C_ADDR, &reg, 1, true);  // true to keep master control of bus
    if (err == PICO_ERROR_GENERIC) {
        return STATUS_FAILED;
    }
    
    // read in one go as register addresses auto-increment
    err = i2c_read_blocking(i2cbus, BME_I2C_ADDR, buf, NUM_CALIB_PARAMS, false);  // false, we're done reading
    if (err == PICO_ERROR_GENERIC) {
        return STATUS_FAILED;
    }
    // store these in a struct for later use
    params->dig_t1 = (uint16_t)(buf[1] << 8) | buf[0];
    params->dig_t2 = (int16_t)(buf[3] << 8) | buf[2];
    params->dig_t3 = (int16_t)(buf[5] << 8) | buf[4];

    params->dig_p1 = (uint16_t)(buf[7] << 8) | buf[6];
    params->dig_p2 = (int16_t)(buf[9] << 8) | buf[8];
    params->dig_p3 = (int16_t)(buf[11] << 8) | buf[10];
    params->dig_p4 = (int16_t)(buf[13] << 8) | buf[12];
    params->dig_p5 = (int16_t)(buf[15] << 8) | buf[14];
    params->dig_p6 = (int16_t)(buf[17] << 8) | buf[16];
    params->dig_p7 = (int16_t)(buf[19] << 8) | buf[18];
    params->dig_p8 = (int16_t)(buf[21] << 8) | buf[20];
    params->dig_p9 = (int16_t)(buf[23] << 8) | buf[22];
}


BME280::BME280() {
    // constructor

}

/**
 * Initialise the device.
 * 
 * Performs any setup required inc. calculating
 * magic numbers etc. - returns a STATUS the same way as checkOK. You can probably
 * just call checkOK at the end of this, but obviously there if you fail the setup steps
 * that's worth thinking about too.
 * 
 * @return status: device status
*/
status BME280::init(bool i2c_bus) {
    // use the "handheld device dynamic" optimal setting (see datasheet)
    uint8_t buf[2];

    // Set bus
    if (i2c_bus) i2cbus = i2c1;
    else i2cbus = i2c0;

    // 500ms sampling time, x16 filter
    const uint8_t reg_config_val = ((0x04 << 5) | (0x05 << 2)) & 0xFC;

    // send register number followed by its corresponding value
    buf[0] = REG_CONFIG;
    buf[1] = reg_config_val;
    printf("pomodorino");
    int err = i2c_write_blocking(i2c0, BME_I2C_ADDR, buf, 2, false);
    if (err == PICO_ERROR_GENERIC) {
        return STATUS_FAILED;
    }
    printf("angelhair");

    // osrs_t x1, osrs_p x4, normal mode operation
    const uint8_t reg_ctrl_meas_val = (0x01 << 5) | (0x03 << 2) | (0x03);
    buf[0] = REG_CTRL_MEAS;
    buf[1] = reg_ctrl_meas_val;
    err = i2c_write_blocking(i2c0, BME_I2C_ADDR, buf, 2, false);
    if (err == PICO_ERROR_GENERIC) {
        return STATUS_FAILED;
    }

    // Generate calibration parameters
    bme280_get_calib_params(&_calib_params);
    
    return STATUS_OK;
}

std::vector<baro_reading_t> BME280::read() {
    auto readings = std::vector<baro_reading_t>();
    
    // Perform raw reads
    int32_t raw_temperature;
    int32_t raw_pressure;
    bme280_read_raw(&raw_temperature, &raw_pressure);
    if (raw_temperature == 0 || raw_pressure == 0) {
        return readings;
    }
    int32_t comp_temp = bme280_convert_temp(raw_temperature, &_calib_params);
    int32_t comp_pressure = bme280_convert_pressure(raw_pressure, raw_temperature, &_calib_params);

    // Insert readings into vector
    baro_reading_t reading;
    reading.temp = comp_temp;
    reading.pressure = comp_pressure;
    readings.push_back(reading);

    return readings;
}

status BME280::checkOK() {
    // Try read from WHOAMI register
    uint8_t reg = REG_WHOAMI;
    uint8_t response[1] = {0};

    int err = i2c_write_blocking(i2cbus, BME_I2C_ADDR, &reg, 1, true);  // true to keep master control of bus
    if (err == PICO_ERROR_GENERIC) {
        return STATUS_FAILED;
    }

    err = i2c_read_blocking(i2cbus, BME_I2C_ADDR, response, 1, false);  // false, we're done reading
    if (err == PICO_ERROR_GENERIC) {
        return STATUS_FAILED;
    }
    printf("Read from WHOAMI: %d\n", response[0]);
    return STATUS_OK;
}