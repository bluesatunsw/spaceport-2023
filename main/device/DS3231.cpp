// DS3231.cpp
// Implementation of DS3231 real time clock.
// [name] [GitHub Username]
// 05/2023

#include "DS3231.hpp"
#include "i2c_cxx.hpp"
#include "types.hpp"
#include <sys/_stdint.h>

// the module reutrns time in a date sorta format
// the utc conversion could maybe be handled here?
struct timeval DS3231::getTime() {
    // Placeholder
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return tv;
}

DS3231::DS3231() {
    // Placeholder
}

/**
 * Check if the device is working correctly.
 * 
 * Returns either STATUS_OK if normal, STATUS_MISBEHAVING if
 * accessible but readings out of range, or STATUS_FAILED otherwise.
 * 
 * @return status: device status
*/
status DS3231::checkOK() {
    // Placeholder
    return alive;
}

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
status DS3231::init(std::shared_ptr<idf::I2CMaster> i2c) {
    // Placeholder
    this->addr = std::make_unique<idf::I2CAddress>(DS3231_I2C_ADDR);
    this->i2c = i2c;

    try {
        this->i2c->sync_read(*this->addr, 0); // send nothing
                                            // should still recieve ACK
    } catch (idf::I2CException& e) {
      // something went wrong with the i2c communication
          alive = STATUS_FAILED;
          return STATUS_FAILED;
    }

    return checkOK();
}

// time registers are BCD.
rtc_datetime_t bytesToTime(uint8_t data[]) {
    rtc_datetime_t t {};

    uint8_t seconds = data[0];
    t.seconds = (seconds & 0b00001111)
        + ((seconds >> 4) & 0b00000111) * 10;

    uint8_t minutes = data[1];
    t.minutes = (minutes & 0b00001111)
        + ((minutes >> 4) & 0b00000111) * 10;

    uint8_t hours = data[2];
    if (hours & 0b0100000) { // 12 hr mode
        t.hours = (hours & 0b00001111)
            + (hours & 0b00010000) * 12;
    } else { // 24 hour mode
        t.hours = (hours & 0b0001111)
            + ((hours >> 4) & 0b00000011) * 10;
    }

    uint8_t day = data[3];
    t.day = day & 0b00000111;

    uint8_t date = data[4];
    t.date = (date & 0b00001111)
        + ((date >> 4) & 0b00000011) * 10;

    uint8_t month = data[5];
    t.date = (month & 0b00001111)
        + ((month >> 4) & 0b00000001) * 10;

    uint8_t year = data[6];
    t.year = (year & 0b00001111)
        + ((year >> 4) & 0b00001111) * 10
        + (month >> 7) * 100; // century bit is stored in month
        // + 2000; // do we need to do this?

    return t;
}

// called on interupt? this chip doesn't really need to be 
// waited on for its data to be ready. it can be read at any time.
// i'm leaving it this way to match up better with the other devices
// but the i2c call can maybe be moved to be directly in the 
// get time and get date time functions
status DS3231::update() {
    try {
        std::vector<uint8_t> start_addr = {0x00};
        auto data = this->i2c->sync_transfer(*addr, start_addr, 7);
        rtcTime = bytesToTime(data.data());
    } catch (idf::I2CException& e) {
        alive = STATUS_FAILED;
    }
    
    return alive;
}


rtc_datetime_t DS3231::getDateTime() {
    return rtcTime;
}

void DS3231::stop()
{

}

void DS3231::watchdog_task(void *parameters)
{

}

void DS3231::watchdog_callback(TimerHandle_t xtimer)
{

}
