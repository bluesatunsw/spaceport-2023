// H3LIS100DLTR.hpp
// Header file for the H3LIS100DLTR accelerometer
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef H3LIS100DLTR_H
#define H3LIS100DLTR_H

#include "Device.hpp"

#define H3LIS100DLTR_I2C_ADDR 0x19

class H3LIS100DLTR : public Device {
public:
    H3LIS100DLTR();

    // Device methods
    status checkOK() override;
    status init();
    std::vector<accel_reading_t> read();

    void stop() override;

protected: 
    // void watchdog_task(void *parameters) override;
    // void watchdog_callback(TimerHandle_t xtimer) override;

private:
    // std::shared_ptr<idf::I2CAddress> addr;
    // std::shared_ptr<idf::I2CMaster> i2c;
};

#endif