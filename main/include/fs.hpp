// fs.hpp
// Header for littlefs filesystem wrapper
// Matt Rossouw (omeh-a)
// 06/2023

#include "pico/stdio.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <sstream>
#include <vector>
#include "System.hpp"
#include "types.hpp"
#include <iomanip>
extern "C" {
    #include "lfs.h"
    #include "lfs_util.h"
}


#define BLOCK_SIZE_BYTES (FLASH_SECTOR_SIZE)
#define HW_FLASH_STORAGE_BYTES  (1408 * 1024)
#define HW_FLASH_STORAGE_BASE   (PICO_FLASH_SIZE_BYTES - HW_FLASH_STORAGE_BYTES) // 655360

int init_fs();
int fs_logmsg(std::string msg);
int fs_log_imu(std::vector<imu_reading_t> readings);
int fs_log_baro(std::vector<baro_reading_t> readings);
int fs_log_acc(std::vector<accel_reading_t> readings);
void shutdown_fs();
void offload(void);