// DS3231.hpp
// Header file for the DS3231 RTC module
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef DS3231_H
#define DS3231_H

#include <sys/time.h>
#include "Device.hpp"


#define DS3231_I2C_ADDR 0x68

// Class prototype
class DS3231 : public Device {
public:
    DS3231();

    // Device methods
    struct timeval getTime();
    status checkOK() override;
    status init();


protected: 
//     void watchdog_task(void *parameters) override;
//     void watchdog_callback(TimerHandle_t xtimer) override;

// private:
//     std::shared_ptr<idf::I2CAddress> addr;
//     std::shared_ptr<idf::I2CMaster> i2c;
};
#endif
