// H3LIS100DLTR.cpp
// Implementation of H3LIS100DLTR accelerometer.
// [name] [GitHub Username]
// 05/2023

#include "H3LIS100DLTR.hpp"

// I2C wrappers for the H3LIS100DLTR

// I2C write from Bufp to register Reg
int32_t platform_write(void *handle, uint8_t Reg, const uint8_t *Bufp, uint16_t len) {
    // Write register handle
    i2c_write_blocking(i2c_default, H3LIS100DLTR_I2C_ADDR, &Reg, 1, true);
    // Write data
    i2c_write_blocking(i2c_default, H3LIS100DLTR_I2C_ADDR, Bufp, len, false);

    return 0;
}

int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len) {
    // Write register handle
    i2c_write_blocking(i2c_default, H3LIS100DLTR_I2C_ADDR, &Reg, 1, true);
    // Read
    i2c_read_blocking(i2c_default, H3LIS100DLTR_I2C_ADDR, Bufp, len, false);
}

stmdev_ctx_t dev_ctx = {
    platform_write,
    platform_read,
    0,
    0
};

H3LIS100DLTR::H3LIS100DLTR() {
    
}

/**
 * Take a reading from this device.
 * @return A vector of readings from this device of type [TYPE]
*/
std::vector<accel_reading_t> H3LIS100DLTR::read() {
    // Get acceleration
    int8_t raw_acceleration[3];
    int32_t ret = h3lis100dl_acceleration_raw_get(&dev_ctx, raw_acceleration);

    // Adjust
    accel_reading_t reading = {
        .acc_x = h3lis100dl_from_fs100g_to_mg(raw_acceleration[0]),
        .acc_y = h3lis100dl_from_fs100g_to_mg(raw_acceleration[1]),
        .acc_z = h3lis100dl_from_fs100g_to_mg(raw_acceleration[2])
    };
    std::vector<accel_reading_t> readings = std::vector<accel_reading_t>({0});
    // Add to vector
    readings.push_back(reading);
    return readings;
}

/**
 * @brief 
 * 
 *
 * 
 * Returns either STATUS_OK if normal, STATUS_MISBEHAVING if
 * accessible but readings out of range, or STATUS_FAILED otherwise.
 * 
 * @return status: device status
**/
status H3LIS100DLTR::checkOK() {
    // Placeholder
    return STATUS_OK;
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
status H3LIS100DLTR::init() {
    // Enable all axes
    h3lis100dl_axis_x_data_set(&dev_ctx, PROPERTY_ENABLE);
    h3lis100dl_axis_y_data_set(&dev_ctx, PROPERTY_ENABLE);
    h3lis100dl_axis_z_data_set(&dev_ctx, PROPERTY_ENABLE);

    // Set output data rate to 100Hz
    h3lis100dl_data_rate_set(&dev_ctx, H3LIS100DL_ODR_100Hz);
    return STATUS_OK;
}
