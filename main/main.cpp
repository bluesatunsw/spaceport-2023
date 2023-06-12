// main.cpp
// Main entrypoint for the OBC firmware for Spaceport Cup 2023.
// Matt Rossouw (omeh-a), [name], [name] ...
// 05/2023

// Standard dependencies
#include <inttypes.h>

// Out of tree dependencies
// if you get compile errors on these check your esp-idf install.
// Your IDE will almost definitely be confused by these, but don't worry.
// #include "sdkconfig.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_chip_info.h"
// #include "esp_flash.h"

// Our dependencies
#include "main.hpp"
#include "System.hpp"

// Globals
System dm;

static int64_t payload_monitor_callback(alarm_id_t id, void *user_data) {
    // Check if payload is OK
    status payload_status = dm.payload.get_status()
    if (payload_status == PAYLOAD_STATUS_IDLE) {
        // Payload is not OK, log error
        dm.log(std::string("Payload OK: idle"), LOG_INFO);
    }
    else if (payload_status == PAYLOAD_STATUS_PRINTING) {
        dm.log(std::string("Payload OK: experiment in progress"), LOG_INFO);
    }
    else if (payload_status == PAYLOAD_STATUS_DONE) {
        dm.log(std::string("Payload OK: "), LOG_INFO);
    }
    else if (payload_status == PAYLOAD_STATUS_FAULT) {
        dm.log(std::string("Payload error: reporting unspecified fault!"), LOG_ERROR);
    }
    else if (payload_status == PAYLOAD_STATUS_OVERHEAT) {
        dm.log(std::string("Payload error: LED array overheat detected!"), LOG_ERROR);
    }
    else if (payload_status == PAYLOAD_STATUS_MOTOR_FAIL) {
        dm.log(std::string("Payload error: Motor failure detected!"), LOG_ERROR);
    }
    else {
        dm.log(std::string("Payload failed to report status!"), LOG_CRITICAL);
    }
    add_alarm_in_ms(2500, payload_monitor_callback, NULL, true);
}

// idf entrypoint
extern "C" int main()
{
    stdio_init_all();
    printf("Initialising Bluesat Rocket Telemetry system...\n");
    obc_main();
}

// Main function
void obc_main(void) {
    // Switch to appropriate mode
    bool mission_mode = false;
    switch (dm.mode) {
        case MODE_NORMAL:
            mission_mode = false;
            break;
        case MODE_TEST:
            mission_mode = true;
            break;
        case MODE_OFFLOAD:
            offload();
            break;
        // default to diagnostic, in case we somehow end up here
        default:
            diagnostic();
            // Diagnostic should never return, but just in case...
            return;
    }
    // Initialise sensors
    dm.sensor_init();


    mission(mission_mode);
}

/**
 * Main mission loop.
 * 
 * Encapsulates all logic for the spaceport mission.
 * 
 * @param test Whether or not to run in test mode. If true,
 *             all logging output will be outputted on serial.
*/
void mission(bool test) {
    dm.await_arm();

    // Add payload monitor
    add_alarm_in_ms(2500, payload_monitor_callback, NULL, true);
    printf("Arming sequence complete\n");
    for (;;) {
        // Adapt to flight stage
        if (dm.flight_stage == STAGE_COAST || dm.flight_stage == STAGE_POWERED_ASCENT
            || dm.flight_stage == STAGE_EXPERIMENT) {
            // Read sensors
            fs_log_baro(dm.baroread());
            fs_log_imu(dm.imuread());
        }
        else if (dm.flight_stage == STAGE_PAD) {
            dm.await_launch();
        }
        else if (dm.flight_stage == STAGE_TERMINATION) {
            // Terminate flight
            break;
        }
    }
    shutdown_fs();

}

/**
 * Offload loop.
 * 
 * Switches the system to act as a USB mass storage device for
 * accessing the data stored on the flash chip.
*/
void offload(void) {
    for (;;) {
        // Placeholder
    }
}

/**
 * Diagnostic loop.
 * 
 * Outputs all sensor output on serial for sanity checking.
*/
void diagnostic(void) {
    for (int j = 0;j < 10;j++) {
        // Diagnostic mode ignores the flash chip. Just read all sensors and print.
        printf("WE ARE IN DIAGNOSTIC FOR LOOP\n ");
        // Accelerometer
        // std::vector<accel_reading_t> accel_readings = dm.accelread();

        // IMU
        // std::vector<imu_reading_t> imu_readings = dm.imuread();

        // Barometer
        std::vector<baro_reading_t> baro_readings = dm.baroread();

        // Grab curr_time
        time_t curr_time = time(NULL);

        // Print all values in one line
        // FIXME: Uncomment this below 
        // printf("%" PRIu64 " ", curr_time);
        // for (int i = 0; i < accel_readings.size(); i++) {
        //     printf("| acc%d    x=[%8u] y=[%8u] z=[%8u] |\n",i, accel_readings[i].acc_x, accel_readings[i].acc_y, accel_readings[i].acc_z);
        // }
        // for (int i = 0; i < imu_readings.size(); i++) {
        //     printf("| imu%d_ac x=[%8u] y=[%8u] z=[%8u] |\n", i, (unsigned)imu_readings[i].acc_x, (unsigned)imu_readings[i].acc_y, (unsigned)imu_readings[i].acc_z);
        //     printf("| imu%d_gy x=[%8u] y=[%8u] z=[%8u] |\n", i, (unsigned)imu_readings[i].gyr_x, (unsigned)imu_readings[i].gyr_y, (unsigned)imu_readings[i].gyr_z);
        //     printf("| imu%d_mg x=[%8u] y=[%8u] z=[%8u] |\n", i, (unsigned)imu_readings[i].mag_x, (unsigned)imu_readings[i].mag_y, (unsigned)imu_readings[i].mag_z);
        // }
        
        //DEBUG STUFF: IZY 
        dm.baroread();
        
        for (int i = 0; i < baro_readings.size(); i++) {
            printf("| baro%d t=[%8u] p=[%8u] ", i, (unsigned)baro_readings[i].temp, (unsigned)baro_readings[i].pressure);
        }
        // TODO: add logic from drivers to convert raw bytes to actual readout

    }
}
