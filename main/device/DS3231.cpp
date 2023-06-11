// DS3231.cpp
// Implementation of DS3231 real time clock.
// [name] [GitHub Username]
// 05/2023

#include "DS3231.hpp"

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



void DS3231::stop()
{

}

