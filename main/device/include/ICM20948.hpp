// ICM20948.hpp
// Header file for the ICM20948 9-axis IMU
// Ibrahim Fuad - (kiesen)
// 05/2023

#ifndef ICM20948_H
#define ICM20948_H

#include "Device.hpp"

class ICM20948 : public Device {
public:
    ICM20948();

    static const uint8_t BASE_ADDRESS = 0x69;

    std::vector<imu_reading_t> read();
    status init(bool alt_address);

    // Device methods
    status checkOK() override;

    void stop() override;

    void update(void);

protected: 
    // void watchdog_task(void *parameters) override;
    // void watchdog_callback(TimerHandle_t xtimer) override;

private:
    // std::unique_ptr<idf::I2CAddress> addr;
    // std::shared_ptr<idf::I2CMaster> i2c;

    std::vector<imu_reading_t> measurements;
};

#endif
