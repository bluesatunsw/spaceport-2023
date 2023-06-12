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
    int f = pico_open("/log.txt", "r");
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
    int f = pico_open("/log.txt", "a");
    if (f == NULL) return -1;

    // Get timestamp from system clock
    uint64_t timestamp = time_us_64();

    // Convert time to 24h format + day of year
    uint8_t hour = (timestamp / 3600000000) % 24;
    uint8_t minute = (timestamp / 60000000) % 60;
    uint8_t second = (timestamp / 1000000) % 60;
    uint16_t millisecond = (timestamp / 1000) % 1000;
    uint16_t day = (timestamp / 86400000000) % 365;

    std::stringstream ss;
    ss << day << " - " << std::setfill('0') << std::setw(2) << hour << ":" 
        << std::setfill('0') << std::setw(2) << minute << ":" 
        << std::setfill('0') << std::setw(2) << second << "." 
        << std::setfill('0') << std::setw(3) << millisecond << ": " << msg << "\n";

    std::string s = ss.str();
    pico_write(f, s.c_str(), s.size());

    pico_close(f);
    return 0;
}

int fs_log_imu(std::vector<imu_reading_t> readings) {
    int f = pico_open("/imu.txt", "a");
    if (f == NULL) return -1;

    // Get timestamp from system clock
    uint64_t timestamp = time_us_64();

    // Convert time to 24h format + day of year
    uint8_t hour = (timestamp / 3600000000) % 24;
    uint8_t minute = (timestamp / 60000000) % 60;
    uint8_t second = (timestamp / 1000000) % 60;
    uint16_t millisecond = (timestamp / 1000) % 1000;
    uint16_t day = (timestamp / 86400000000) % 365;

    std::stringstream ss;
    // Log timestamp + contents of reading vector
    ss << day << " - " << std::setfill('0') << std::setw(2) << hour << ":" 
        << std::setfill('0') << std::setw(2) << minute << ":" 
        << std::setfill('0') << std::setw(2) << second << "." 
        << std::setfill('0') << std::setw(3) << millisecond << ",";
    
    for (auto reading : readings) {
        ss << reading.acc_x << "," << reading.acc_y << "," << reading.acc_z 
            << "," << reading.gyr_x << "," << reading.gyr_y << "," << reading.gyr_z << ",";
    }
    ss << "\n";

    std::string s = ss.str();
    pico_write(f, s.c_str(), s.size());

    pico_close(f);
    return 0;
}

int fs_log_baro(std::vector<baro_reading_t> readings) {
    int f = pico_open("/baro.txt", "a");
    if (f == NULL) return -1;

    // Get timestamp from system clock
    uint64_t timestamp = time_us_64();

    // Convert time to 24h format + day of year
    uint8_t hour = (timestamp / 3600000000) % 24;
    uint8_t minute = (timestamp / 60000000) % 60;
    uint8_t second = (timestamp / 1000000) % 60;
    uint16_t millisecond = (timestamp / 1000) % 1000;
    uint16_t day = (timestamp / 86400000000) % 365;

    std::stringstream ss;
    ss << day << " - " << std::setfill('0') << std::setw(2) << hour << ":" 
        << std::setfill('0') << std::setw(2) << minute << ":" 
        << std::setfill('0') << std::setw(2) << second << "." 
        << std::setfill('0') << std::setw(3) << millisecond << ",";
    
    for (auto reading : readings) {
        ss << reading.temp << "," << reading.pressure << "," << reading.altitude << "\n";
    }
    
    ss << "\n";

    std::string s = ss.str();
    pico_write(f, s.c_str(), s.size());

    pico_close(f);
    return 0;
}

void shutdown_fs() {
    // Note shutdown in logfile
    fs_logmsg(std::string("Shutting down filesystem."));
    pico_unmount();
}