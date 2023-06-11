// ESPFlash.cpp
// Functions controlling the ESP32's internal flash.
// [name] [github usernmae]
// 05/2023

#include "ESPFlash.hpp"

ESPFlash::ESPFlash() {
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
status ESPFlash::checkOK() {
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
status ESPFlash::init() {
    // Placeholder
    return STATUS_OK;
// bool ESPFlash::checkOK() {
//     if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
//     {
//         printf("Get flash size failed");
//         return;
//     }

//     printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
//            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

//     printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
// }
}


void ESPFlash::stop()
{

}
