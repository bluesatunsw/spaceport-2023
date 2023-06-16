// ESPFlash.hpp
// Header for ESP32's internal flash.
// [name] [github usernmae]
// 05/2023

#ifndef ESPFLASH_H
#define ESPFLASH_H

#include "Device.hpp"

class ESPFlash : public Device {
public:
    ESPFlash();

    // Device methods
    status checkOK() override;
    status init();


protected: 
    // void watchdog_task(void *parameters) override;
    // void watchdog_callback(TimerHandle_t xtimer) override;
};

#endif