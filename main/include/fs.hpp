// fs.hpp
// Header for littlefs filesystem wrapper
// Matt Rossouw (omeh-a)
// 06/2023

#include "pico/stdio.h"
#include <sstream>
#include "pico_hal.h"


int init_fs();
int fs_logmsg(std::string msg);
int fs_log_imu(std::vector<imu_reading_t> readings);
int fs_log_baro(std::vector<baro_reading_t> readings);
void shutdown_fs();