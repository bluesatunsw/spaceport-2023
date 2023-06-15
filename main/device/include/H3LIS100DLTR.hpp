// H3LIS100DLTR.hpp
// Header file for the H3LIS100DLTR accelerometer
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef H3LIS100DLTR_H
#define H3LIS100DLTR_H

#include "Device.hpp"

#define H3LIS100DLTR_I2C_ADDR 0x19
#define H3LIS100DLTR_R_STATUS 0x27


class H3LIS100DLTR : public Device {
public:
    H3LIS100DLTR();

    // Device methods
    status checkOK() override;
    status init();
    std::vector<accel_reading_t> read();


};

#endif