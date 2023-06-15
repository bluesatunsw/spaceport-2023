// fs.cpp
// Wrapper around littleFS for RP2040
// Matt Rossouw (omeh-a)
// 06/2023

#include "fs.hpp"

#define FS_DEBUG

lfs_t lfs;


int pico_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t fs_start = XIP_BASE + HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * c->block_size) + off;
    
    // printf("[FS] READ: %p, %d\n", addr, size);
    
    memcpy(buffer, (unsigned char *)addr, size);
    return 0;
}

int pico_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * c->block_size) + off;
    
    // printf("[FS] WRITE: %p, %d\n", addr, size);
        
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(addr, (const uint8_t *)buffer, size);
    restore_interrupts(ints);
    
    return 0;
}

int pico_erase(const struct lfs_config *c, lfs_block_t block)
{           
    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t offset = fs_start + (block * c->block_size);
    
    // printf("[FS] ERASE: %p, %d\n", offset, block);
        
    uint32_t ints = save_and_disable_interrupts();   
    flash_range_erase(offset, c->block_size);  
    restore_interrupts(ints);
    
    return 0;
}

int pico_sync(const struct lfs_config *c)
{
    return 0;
}

// configuration of the filesystem is provided by this struct
const struct lfs_config FLASH_CONFIG = {
    // block device operations
    .read  = &pico_read,
    .prog  = &pico_prog,
    .erase = &pico_erase,
    .sync  = &pico_sync,

    // block device configuration
    .read_size = FLASH_PAGE_SIZE, // 256
    .prog_size = FLASH_PAGE_SIZE, // 256
    
    .block_size = BLOCK_SIZE_BYTES, // 4096
    .block_count = HW_FLASH_STORAGE_BYTES / BLOCK_SIZE_BYTES, // 352
    .block_cycles = 16, // ?
    
    .cache_size = FLASH_PAGE_SIZE, // 256
    .lookahead_size = FLASH_PAGE_SIZE,   // 256    
};

// ### OUR STUFF ###

// /**
//  * Initialise flash and mount filesystem.
//  * @return 0 on success, 1 on failure.
// */
int init_fs() {
    printf("Mounting flash... \n");
    int ret = lfs_mount(&lfs, &FLASH_CONFIG);
    // Format if mount fails
    if (ret) {
        printf("Mount failed, formatting... \n");
        lfs_format(&lfs, &FLASH_CONFIG);
        ret = lfs_mount(&lfs, &FLASH_CONFIG);
        if (ret) return 1; // Failed to mount
    }
    printf("Format successful. \n");

    lfs_file_t file;
    // Check if log.txt, imu.txt and baro.txt exist. Create otherwise.

    int f = lfs_file_open(&lfs, &file, "/log.txt", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_close(&lfs, &file);

    f = lfs_file_open(&lfs, &file, "/baro.txt", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_close(&lfs, &file);

    f = lfs_file_open(&lfs, &file, "/acc.txt", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_close(&lfs, &file);
    
    // fs_logmsg(std::string("Filesystem initialised."));
    return 0;
}

/**
 * Log a message to the log file.
 * @param msg The message to log.
 * @return 0 on success, -1 on failure.
*/
int fs_logmsg(std::string msg) {
    lfs_file_t file;
    int f = lfs_file_open(&lfs, &file, "/log.txt", LFS_O_APPEND);
    if (f < 0) return -1;

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
    lfs_file_write(&lfs, &file, s.c_str(), s.size());

    lfs_file_close(&lfs, &file);
    return 0;
}

int fs_log_imu(std::vector<imu_reading_t> readings) {
    lfs_file_t file;
    int f = lfs_file_open(&lfs, &file, "/imu.txt", LFS_O_APPEND);
    if (f < 0) return -1;

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
    lfs_file_write(&lfs, &file, s.c_str(), s.size());

    lfs_file_close(&lfs, &file);
    return 0;
}

int fs_log_baro(std::vector<baro_reading_t> readings) {
    lfs_file_t file;
    int f = lfs_file_open(&lfs, &file, "/baro.txt", LFS_O_APPEND);
    if (f < 0) return -1;

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
        ss << reading.temp << "," << reading.pressure << "," << "\n";
    }
    
    ss << "\n";

    std::string s = ss.str();
    lfs_file_write(&lfs, &file, s.c_str(), s.size());

    lfs_file_close(&lfs, &file);
    return 0;
}

void shutdown_fs() {
    // Note shutdown in logfile
    fs_logmsg(std::string("Shutting down filesystem."));
    lfs_unmount(&lfs);
}

/**
 * Offload loop.
 * 
 * Dumps files from littlefs over UART in raw bytes. Each file is prepended with
 * "\\FILE START [name]\\" and appended with "\\FILE END\\".
 * 
 * After all files have been offloaded, the system will terminate.
*/
void offload(void) {
    // First: get FS size
    lfs_ssize_t fs_size = lfs_fs_size(&lfs);
    // printf("Filesystem online. Block size: %d, total blocks: %d, used blocks %d\n", SECTOR_SIZE, BLOCK_COUNT, fs_size);
    
    // Second: request list of files from root of FS from littlefs
    lfs_dir_t dir;
    int ret = lfs_dir_open(&lfs, &dir, "/");
    if (ret < 0) {
        printf("Error: could not open root directory!\n");
        return;
    }

    // Third: iterate through files and print to serial
    struct lfs_info finfo;
    lfs_file_t file;
    while (lfs_dir_read(&lfs, &dir, &finfo) != 0) {
        // Print file name
        printf("File: %s\n", finfo.name);
        // Open file
        ret = lfs_file_open(&lfs, &file, finfo.name, LFS_O_RDONLY);
        if (ret < 0) {
            printf("Error: could not open file %s!\n", finfo.name);
            return;
        }
        // Print file start
        printf("\\FILE START %s\\\n", finfo.name);
        // Print file contents
        char buf[256];
        while (lfs_file_read(&lfs, &file, buf, 256) > 0) {
            printf("%s", buf);
        }
        // Print file end
        printf("\n \\FILE END\\\n");
        // Close file
        lfs_file_close(&lfs, &file);
    }

    // Close directory
    lfs_dir_close(&lfs, &dir);

    // Unmount filesystem and exit
    lfs_unmount(&lfs);
}