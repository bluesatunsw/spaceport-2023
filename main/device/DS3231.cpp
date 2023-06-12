// DS3231.cpp
// Implementation of DS3231 real time clock.
// [name] [GitHub Username]
// 05/2023

#include "DS3231.hpp"
#include "hardware/i2c.h"
#include "types.hpp"

rtc_reading_t bytesToTime(uint8_t data[]) {
    struct tm t {};
    // rtc_reading_t t {};

    uint8_t seconds = data[0];
    t.tm_sec = (seconds & 0b00001111)
        + ((seconds >> 4) & 0b00000111) * 10;

    uint8_t minutes = data[1];
    t.tm_min = (minutes & 0b00001111)
        + ((minutes >> 4) & 0b00000111) * 10;

    uint8_t hours = data[2];
    if (hours & 0b0100000) { // 12 hr mode
        t.tm_hour = (hours & 0b00001111)
            + (hours & 0b00010000) * 12;
    } else { // 24 hour mode
        t.tm_hour = (hours & 0b0001111)
            + ((hours >> 4) & 0b00000011) * 10;
    }

    uint8_t day = data[3];
    t.tm_wday = day & 0b00000111;

    uint8_t date = data[4];
    t.tm_mday = (date & 0b00001111)
        + ((date >> 4) & 0b00000011) * 10;

    uint8_t month = data[5];
    t.tm_mon = (month & 0b00001111)
        + ((month >> 4) & 0b00000001) * 10;

    uint8_t year = data[6];
    t.tm_year = (year & 0b00001111)
        + ((year >> 4) & 0b00001111) * 10
        + (month >> 7) * 100 // century bit is stored in month
        + 100; // because c expects offset from 1900?

    return t;
}

struct timeval DS3231::getTime() {

    uint8_t reg = 0x00;
    i2c_write_blocking(i2c0, DS3231_I2C_ADDR, &reg, 1, true);

    uint8_t buf[7] {};
    i2c_read_blocking(i2c0, DS3231_I2C_ADDR, buf, 7, false);

    auto datetime = bytesToTime(buf);

    // i think this might need the time zone to be set
    time_t unix_time = mktime(&datetime);

    struct timeval tv {.tv_sec=unix_time, .tv_usec=0};
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
    return STATUS_OK;
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
status DS3231::init() {
    // Placeholder
    return STATUS_OK;
}
