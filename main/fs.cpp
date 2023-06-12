// fs.cpp
// Wrapper around littleFS for RP2040
// Matt Rossouw (omeh-a)
// 06/2023

#include "fs.hpp"

/**
 * Initialise flash and mount filesystem.
*/
int init_fs() {
    int ret = pico_mount(false);
    if (ret == LFS_ERR_OK) return -1;

    // Check if log.txt, imu.txt and baro.txt exist. Create otherwise. 
    FILE *f = pico_open("/log.txt", "r");
    if (f == NULL) {
        f = pico_open("/log.txt", "w");
        pico_close(f);
    }

    f = pico_open("/imu.txt", "r");
    if (f == NULL) {
        f = pico_open("/imu.txt", "w");
        pico_close(f);
    }

    f = pico_open("/baro.txt", "r");
    if (f == NULL) {
        f = pico_open("/baro.txt", "w");
        pico_close(f);
    }


}

/**
 * Log a message to the log file.
 * @param msg The message to log.
 * @return 0 on success, -1 on failure.
*/
int fs_logmsg(std::string msg) {
    FILE *f = pico_open("/log.txt", "a");
    if (f == NULL) return -1;
    fprintf(f, "%s\n", msg.c_str());
    pico_close(f);
    return 0;
}

int log_imu(std::vector<imu_reading_t> readings) {
    FILE *f = pico_open("/imu.txt", "a");
    if (f == NULL) return -1;

    // Get timestamp from system clock
    uint64_t timestamp = time_us_64();

    // Convert time to 24h format + day of year
    uint8_t hour = (timestamp / 3600000000) % 24;
    uint8_t minute = (timestamp / 60000000) % 60;
    uint8_t second = (timestamp / 1000000) % 60;
    uint16_t millisecond = (timestamp / 1000) % 1000;
    uint16_t day = (timestamp / 86400000000) % 365;

    // Log timestamp + contents of reading vector
    fprintf(f, "%d - %02d:%02d:%02d.%03d,", day, hour, minute, second, millisecond);
    for (auto reading : readings) {
        fprintf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", reading.acc_x, reading.acc_y, reading.acc_z, reading.gyr_x, reading.gyr_y, reading.gyr_z);
    }
    fprintf(f, "\n");

    pico_close(f);
    return 0;
}

int log_baro(std::vector<baro_reading_t> readings) {
    FILE *f = pico_open("/baro.txt", "a");
    if (f == NULL) return -1;

    // Get timestamp from system clock
    uint64_t timestamp = time_us_64();

    // Convert time to 24h format + day of year
    uint8_t hour = (timestamp / 3600000000) % 24;
    uint8_t minute = (timestamp / 60000000) % 60;
    uint8_t second = (timestamp / 1000000) % 60;
    uint16_t millisecond = (timestamp / 1000) % 1000;
    uint16_t day = (timestamp / 86400000000) % 365;


    fprintf(f, "%d - %02d:%02d:%02d.%03d,", day, hour, minute, second, millisecond);
    for (auto reading : readings) {
        fprintf(f, "%f,%f,%f,%f\n", reading.temp, reading.pressure, reading.altitude);
    }
    fprintf(f, "\n");

    pico_close(f);
    return 0;
}

