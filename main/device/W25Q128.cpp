// W25Q128.cpp
// Implementation of W25Q128 flash chip.
// [name] [GitHub Username]
// 05/2023

#include "W25Q128.hpp"

W25Q128::W25Q128() {
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
status W25Q128::checkOK() {
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
status W25Q128::init() {
    // Placeholder
    return STATUS_OK;
}


