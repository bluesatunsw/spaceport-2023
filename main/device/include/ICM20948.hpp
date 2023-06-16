// // ICM20948.hpp
// // Header file for the ICM20948 9-axis IMU
// // Ibrahim Fuad - (kiesen)
// // 05/2023

// #ifndef ICM20948_H
// #define ICM20948_H

// #include "Device.hpp"
// #include "icm20948_api.h"
// #include "pico/stdlib.h"
// #include "hardware/i2c.h"

// #define ICM_I2C_ADDR _u(0x68)

// class ICM20948 : public Device {
// public:
//     ICM20948();
//     i2c_inst_t *i2cbus;

//     std::vector<imu_reading_t> read();
//     status init(bool alt_address);

//     // Device methods
//     status checkOK() override;
//     void update(void);

// private:

// };
// static int8_t usr_write(const uint8_t addr, const uint8_t *data, const uint32_t len);
// static int8_t usr_read(const uint8_t addr, uint8_t *data, const uint32_t len);
// static void usr_delay_us(uint32_t period);

// #endif
