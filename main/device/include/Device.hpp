// device.hpp
// Specifies the interface for devices. Device class is inherited
// by all devices. All devices must implement the methods specified

#ifndef DEVICE_H
#define DEVICE_H


#include <stdint.h>
#include <vector>
#include <iostream>
#include <memory>

#include "types.hpp"

enum device_bus {
    BUS_I2C0,
    BUS_I2C1,
    BUS_SPI0,
    BUS_SPI1,
    BUS_UART
};

#define I2C_BUS i2c0

class Device {
public:
    // Runs some test to check the device is working.
    // This might include checking it is accessible on the bus,
    // seeing if measurements are in range or checking status registers.
    virtual status checkOK() = 0;
    
    // Initialise device. Find device on the bus and set up registers, and finalise
    // by running a sanity check (probably using Device::checkOK).
    // Returns a `status`, same as checkOK.
    // virtual status init(idf::I2CMaster) = 0;
    

    // FYI: ignore the "=0" after these methods. It just forced them to be purely virtual.

    bool alive;
};
#endif
