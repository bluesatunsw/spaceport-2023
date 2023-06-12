// Payload.cpp
// 3D printer payload SPI interface
// Matt Rossouw (omeh-a)
// 05/2023

#include "Payload.hpp"
#include "System.hpp"

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}

/**
 * Default constructor for Payload class
*/
Payload::Payload() {

}

/**
 * Check if the payload is OK
 * @return status of the payload
*/
status Payload::checkOK() {
    // Create an SPI transaction which sends OP_CHECKOK
    // and expects PAYLOAD_OK in return
    uint8_t reg = PAYLOAD_OP_CHECKOK;
    uint8_t buf[2];
    cs_select();
    spi_write_blocking(spi0, &reg, 1);
    sleep_ms(10);
    spi_read_blocking(spi0, 0, buf, 2);
    cs_deselect();
    sleep_ms(10);

    // Check response is correct
    uint16_t response = (buf[0] << 8) | buf[1];
    if (response == PAYLOAD_OK) {
        return STATUS_OK;
    }
    return STATUS_FAILED;
}

/**
 * Get the status of the payload
 * @return status of the payload
*/
uint16_t Payload::get_status() {
    // Create an SPI transaction which sends OP_GETSTATUS
    // and expects a 2-byte status in return
    uint8_t reg = PAYLOAD_OP_GETSTATUS;
    uint8_t buf[2];
    cs_select();
    spi_write_blocking(spi0, &reg, 1);
    sleep_ms(10);
    int read = spi_read_blocking(spi0, 0, buf, 2);
    cs_deselect();
    sleep_ms(10);
    if (read == 0) return 0;
    // Check response is correct
    uint16_t response = (buf[0] << 8) | buf[1];
    return response;
}

/**
 * Start the payload
 * @return status of the payload
*/
int Payload::start() {
    // Create an SPI transaction which sends OP_START
    // and expects PAYLOAD_OK in return
    uint8_t reg = PAYLOAD_OP_START;
    uint8_t buf[2];
    cs_select();
    spi_write_blocking(spi0, &reg, 1);
    sleep_ms(10);
    spi_read_blocking(spi0, 0, buf, 2);
    cs_deselect();
    sleep_ms(10);

    // Check response is correct
    uint16_t response = (buf[0] << 8) | buf[1];
    if (response == PAYLOAD_ACK) {
        return 0;
    }
    return -1;
}

/**
 * Stop the payload
 * @return status of the payload
*/
int Payload::stop() {
    // Create an SPI transaction which sends OP_STOP
    // and expects PAYLOAD_OK in return
    uint8_t reg = PAYLOAD_OP_STOP;
    uint8_t buf[2];
    cs_select();
    spi_write_blocking(spi0, &reg, 1);
    sleep_ms(10);
    spi_read_blocking(spi0, 0, buf, 2);
    cs_deselect();
    sleep_ms(10);

    // Check response is correct
    uint16_t response = (buf[0] << 8) | buf[1];
    if (response == PAYLOAD_ACK) {
        return 0;
    }
    return -1;
}

/**
 * Initialise the payload
 * @return status of the payload
*/
status Payload::init() {
    // Expects that SPI bus is already initialised
    return checkOK();
}