// H3LIS100DLTR.cpp
// Implementation of H3LIS100DLTR accelerometer.
// [name] [GitHub Username]
// 05/2023

#include "H3LIS100DLTR.hpp"

H3LIS100DLTR::H3LIS100DLTR() {
    // Placeholder
}

/**
 * Take a reading from this device.
 * @return A vector of readings from this device of type [TYPE]
*/
std::vector<accel_reading_t> H3LIS100DLTR::read() {
    // Placeholder
    std::vector<accel_reading_t> readings = std::vector<accel_reading_t>({0});
    return readings;
}

/**
 * @brief 
 * 
 *
 * 
 * Returns either STATUS_OK if normal, STATUS_MISBEHAVING if
 * accessible but readings out of range, or STATUS_FAILED otherwise.
 * 
 * @return status: device status
**/
status H3LIS100DLTR::checkOK() {
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
status H3LIS100DLTR::init() {
    // Placeholder
    return STATUS_OK;
}


void H3LIS100DLTR::stop()
{

}
