// Payload.hpp
// Header for SPI interface to payload
// Matt Rossouw (omeh-a)
// 06/2023

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "Device.hpp"
#include "hardware/spi.h"

// Magic numbers
#define PAYLOAD_ACK                 0xF1F1
#define PAYLOAD_FAIL                0xE1E1
#define PAYLOAD_OK                  0xB14E
#define PAYLOAD_STATUS_IDLE         0x01
#define PAYLOAD_STATUS_PRINTING     0x02
#define PAYLOAD_STATUS_DONE         0x04
#define PAYLOAD_STATUS_FAULT        0x08
#define PAYLOAD_STATUS_OVERHEAT     0x10
#define PAYLOAD_STATUS_MOTOR_FAIL   0x11

// Operations
#define PAYLOAD_OP_CHECKOK          0x44
#define PAYLOAD_OP_GETSTATUS        0x33
#define PAYLOAD_OP_START            0x22
#define PAYLOAD_OP_STOP             0x11


class Payload : public Device {
public:
    Payload();

    // Device methods
    status checkOK() override;
    status init();

    uint16_t get_status();
    int start();
    int stop();

};

#endif