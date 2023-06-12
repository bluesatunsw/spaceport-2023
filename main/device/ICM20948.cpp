// ICM20948.cpp
// Implementation for the ICM20948 9-axis IMU
// Matt Rossouw (omeh-a)
// 06/2023

#include "ICM20948.hpp"

static i2c_inst_t *curr_i2cbus; // this upsets me greatly

// C interface for ICM driver from Stephen Murphy
static int8_t usr_write(const uint8_t addr, const uint8_t *data, const uint32_t len) {
    icm20948_return_code_t ret = ICM20948_RET_OK;
    uint8_t reg = addr;

    // Write the address
    i2c_write_blocking(curr_i2cbus, ICM_I2C_ADDR, &reg, 1, true);
    
    // Write the data from the provided data buffer
    int err = i2c_write_blocking(curr_i2cbus, ICM_I2C_ADDR, data, len, false);
    if (err == PICO_ERROR_GENERIC) return ICM20948_RET_GEN_FAIL;
    return ret;
}

static int8_t usr_read(const uint8_t addr, uint8_t *data, const uint32_t len) {
    icm20948_return_code_t ret = ICM20948_RET_OK;

    uint8_t reg = addr;

    // Write the address
    i2c_write_blocking(curr_i2cbus, ICM_I2C_ADDR, &reg, 1, true);

    // Read out the data, placing the result in the data buffer
    int err = i2c_read_blocking(curr_i2cbus, ICM_I2C_ADDR, data, len, false);

    return ret;
}

static void usr_delay_us(uint32_t period) {
    sleep_us(period);
}

/**
 * Default constructor for ICM20948
*/
ICM20948::ICM20948() {
    
}

/**
 * Take a reading from this device.
 * @return A vector of readings from this device of type imu_reading_t
*/
std::vector<imu_reading_t> ICM20948::read() {
    std::vector<imu_reading_t> readings;
    curr_i2cbus = i2cbus;   // reassign static i2cbus variable to fool
                            // the horrible function pointer interface.

    // API read structs
    icm20948_accel_t accel;
    icm20948_gyro_t gyro;

    // Call API read function
    icm20948_return_code_t ret = icm20948_getAccelData(&accel);
    if (ret != ICM20948_RET_OK) return readings;
    
    ret = icm20948_getGyroData(&gyro);
    if (ret != ICM20948_RET_OK) return readings;

    // Unpack the readings into an `imu_reading_t` struct
    imu_reading_t reading;
    reading.acc_x = accel.x;
    reading.acc_y = accel.y;
    reading.acc_z = accel.z;
    reading.gyr_x = gyro.x;
    reading.gyr_y = gyro.y;
    reading.gyr_z = gyro.z;

    
    return readings;
}

/**
 * Check if the device is working correctly.
 * 
 * Returns either STATUS_OK if normal, STATUS_MISBEHAVING if
 * accessible but readings out of range, or STATUS_FAILED otherwise.
 * 
 * @return status: device status
*/
status ICM20948::checkOK() {
    // Placeholder
    return STATUS_OK;
}

#define USER_CTRL 0x03
#define INT_ENABLE 0x10

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
status ICM20948::init(bool alt_address) {
    if (alt_address) i2cbus = i2c1;
    else i2cbus = i2c0;

    curr_i2cbus = i2cbus;
    // Invoke API init function
    icm20948_return_code_t ret = icm20948_init(usr_read, usr_write, usr_delay_us);
    
    // Configure the device
    if( ret == ICM20948_RET_OK ) {
        icm20948_settings_t settings;
        // Enable the Gyro
        settings.gyro.en = ICM20948_MOD_ENABLED;
        // Select the +-20000dps range
        settings.gyro.fs = ICM20948_GYRO_FS_SEL_2000DPS;
        // Enable the Accel
        settings.accel.en = ICM20948_MOD_ENABLED;
        // Select the +-2G range
        settings.accel.fs = ICM20948_ACCEL_FS_SEL_16G;
        ret = icm20948_applySettings(&settings);
    } else {
        return STATUS_FAILED;
    } 

    return STATUS_OK;
}

