// System.cpp
// Manages all device accesses and IO in a fault-resilient way.
// Failed devices are automatically brought offline and regularly
// retested. Active devices have their readings checked regularly.
// Matt Rossouw (omeh-a)
// 05/2023

#include "System.hpp"

/**
 * Default constructor for the system class.
 * Encaspualtes all initialisation logic. This technically should be
 * a singleton class but I think nobody is going to go crazy with this.
 */
System::System() {
    

    // // Check jumpers to decide system mode.
    // // This line of code is potentially nefarious because I'm not too sure
    // // how C++ handles enum types
    // mode = (system_mode)(gpio_get_level(PIN_OFFLOAD) | (gpio_get_level(PIN_TESTMODE) << 1));


    // Initialise logger
    log_init();
    
    // TODO: mount filesystem

    // Check if RTC is connected
    // Timezone is hardcoded to UTC because we don't really care about it.
    struct timeval tv;
    if (rtc.checkOK()) {
        // If OK, set system time to RTC time
        tv = rtc.getTime();
        settimeofday(&tv, NULL);
        log_internal(std::string("RTC found. Setting system time to RTC time.\n"), LOG_INFO);
    } else {
        // Otherwise, leave system time as is.
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        log_internal(std::string("RTC not found. Falling back to relative time.\n"), LOG_WARNING);
    }
    // NOTE: We settimeofday inside of both branches of the if statement so
    //       our logging system can use the system time.

    log_internal(std::string("Core initialisation complete.\n"), LOG_INFO);
}



/**
 * Attempts to initialise all connected sensors.
 * Must be called before attempting to read sensors.
 * 
 * @note System::i2c_init must succeed before calling this.
 */
void System::sensor_init() {
    // I2C devices
    #ifdef I2C_ENABLE
    // Initialise GPIO pins for system control
    gpio_set_function(PIN_SCL0, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SDA0, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SCL0);
    gpio_pull_up(PIN_SDA0);
    i2c_init(i2c0, 400000);
    status err = baro0.init(false);
    if (err == STATUS_FAILED) {
        log_internal(std::string("Barometer 0 failed to initialise.\n"), LOG_WARNING);
    }

    baro0.checkOK();
    #endif
    #ifdef SPI_ENABLE
    // Set up SPI bus
    spi_init(spi_default, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    // Devices


    #endif
    // imu0.init(this->i2c, false);
    // imu1.init(this->i2c, true);
}

void System::sensor_update() {
    // System::data_ready.acquire(); // block until data is ready

    // imu0.update();
    // imu1.update();
}

/**
 * Attempts to take a reading from each working accelerometer. 
 * 
 * @return std::vector<accel_reading_t> containing readings for each accelerometer
 */
std::vector<accel_reading_t> System::accelread(void) {
    // Placeholder
    return std::vector<accel_reading_t>({0});
}
/**
 * Attempts to take a reading for each working IMU.
 *
 * @return std::vector<imu_reading_t> containing readings for each imu
 */
std::vector<imu_reading_t> System::imuread(void) {
    // Placeholder

    return std::vector<imu_reading_t>({0});
}

/**
 * Attempts to take a reading for each working IMU.
 *
 * @return std::vector<baro_reading_t> containing readings for each barometer
 */
std::vector<baro_reading_t> System::baroread(void) {
    // Placeholder

    return std::vector<baro_reading_t>({0});
}

/**
 * Logs a message to the system log.
 * 
 * Automatically arbitrates between internal and external flash, and
 * will also output to serial if the system is in test mode or 
 * diagnostic mode.
 * 
 * @param msg The message to log.
*/
void System::log_msg(std::string msg, log_type type) {
    // Placeholder
    std::cout << msg;
}

/**
 * Logs a message to the system log. Same as `log_msg` but prepends
 * the source as coming from the System manager.
 * 
 * Automatically arbitrates between internal and external flash, and
 * will also output to serial if the system is in test mode or 
 * diagnostic mode.
 * 
 * @param msg The message to log.
*/
void System::log_internal(std::string msg, log_type type) {
    // Placeholder
    std::cout << "System: " << msg;
}

/**
 * Initialises the system logger.
 * 
 * Generates a new logfile if appropriate on the correct
 * device.
*/
void System::log_init() {
    // Placeholder
    std::cout << "Initialising logger...\n";
}

/**
 * Silently wait until launch is detected.
 * 
 * Launch is detected with a >1G vertical acceleration and a vertical velocity
 * in the appropriate direction.
 * If the high range accelerometer is unavailable the IMU takes over.
 * 
 * NOTE: velocity not yet implemented
 */
void System::await_launch(void) {

    uint8_t prev = 0;
    // Sit in loop
    // for (;;) {
    //     auto acc_readings = accelread();
    //     uint8_t acc_out = 0;
    //     // Take average of accelerometer read
    //     for (int i = 0; i < acc_readings.size(); i++) {
    //         acc_out = (acc_out + acc_readings[i].acc_z) / 2;
    //     }

    //     auto imu_readings = imuread();
    //     // Take average of IMU read
    //     if (acc_readings.size() == 0) {
    //         for (int i = 0; i < imu_readings.size(); i++) {
    //             // If accelerometer unavailable, overwrite acc reading
    //             acc_out = (acc_out + imu_readings[i].acc_z) / 2;
    //         }
    //     }
    //     // Decide if rate of change sufficient for launch
    //     // TODO: increase sample size if needed / include velocity calculation
    //     // Note: converting accel launch threshold to be for 50ms (50ms = 1/20 seconds)
    //     if (acc_out - prev > (ACCEL_LAUNCH_THRESH/(1000 / LAUNCH_AWAIT_MS))) {
    //         // Launch detected
    //         break;
    //     } else {
    //         prev = acc_out;
    //     }
    
    //     // Delay 50ms
    //     sleep_ms(LAUNCH_AWAIT_MS);
    //     break;
    // }
    log_msg(std::string("Launch detected!\n"), LOG_INFO);
    add_alarm_in_ms(BURNOUT_PAYLOAD_DELAY_MS, staging_callback, this, false);
    flight_stage = STAGE_POWERED_ASCENT;
}

/**
 * Wait for the arming period to elapse. 
 * 
 */
void System::await_arm(void) {
    // Placeholder
    log_msg(std::string("Awaiting arm - 10 minutes\n"), LOG_INFO);
    for (int i = 0; i < ARM_DELAY_SECONDS; i++) {
        sleep_ms(1000);
        printf("%d\n", i);
    }
    log_msg(std::string("Arm period elapsed\n"), LOG_INFO);
}


/**
 * Callback used for flight staging to switch from ascent to coast phase, 
 * and then finally to trigger payload activation.
 * 
 * Initially registered by the end of the launch detection stage (i.e.
 * when the rocket launches) and counts down till the end of powered ascent
 * at approx 6.5 seconds.
 * 
 * This re-registers the timer to wait another 10 seconds to trigger payload.
*/
static int64_t staging_callback(alarm_id_t id, void *user_data) {
    
    System *sys = (System *)user_data;
    if (sys->flight_stage == STAGE_POWERED_ASCENT) {
        sys->log_msg("Staging callback triggered, switching to coast mode\n", LOG_INFO);
        sys->flight_stage = STAGE_COAST;

        // Re-register timer for 10 seconds
        add_alarm_in_ms(BURNOUT_PAYLOAD_DELAY_MS, staging_callback, sys, false);

    } else if (sys->flight_stage == STAGE_COAST) {
        sys->log_msg("Staging callback triggered, activating payload!\n", LOG_INFO);
    }
    return 0;
}