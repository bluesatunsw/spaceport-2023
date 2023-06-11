// BME280.hpp
// Header file for the BME280 pressure sensor
// Matt Rossouw (omeh-a)
// 05/2023

#ifndef BME280_H
#define BME280_H

#include "Device.hpp"
#include <stdint.h>



/**
   some :IMPORTANT: notes :)


Sleep mode: no operation, all registers accessible, lowest power, selected after startup
Forced mode: perform one measurement, store results and return to sleep mode
Normal mode: perpetual cycling of measurements and inactive periods.


*/



// just a check for floats
#define BME280_ALLOW_FLOAT (0)


// if it isnt 0 then set it to double.
#if (BME280_ALLOW_FLOAT != 0)
   typedef double temp_t;
   typedef double press_t;
   typedef double humid_t;


// else set it to integer
#else
   typedef int32_t temp_t;
   typedef uint32_t press_t;
   typedef uint32_t humid_t;


#endif /* BME280_ALLOW_FLOAT*/




/***
* Only 2 possible addresses, connecting
* SDO to GND results in slave address 1110110
* (0x76); connecting it to Vddio results in slave address
* 1110111 (0x77) which is the same as BMP280's I2C
* address
*
* IMPORTANT: The SDO pin cannot be left floating; if left
* floating, the I²C address will be
* undefined.
*/
#define BME280_I2C_ADDRESS1 (0x76)
#define BME280_I2C_ADDRESS2 (0x77)


// Calibration registers
#define BME280_CAL_T1  (0x88)
#define BME280_CAL_T2  (0x8a)
#define BME280_CAL_T3  (0x8c)
#define BME280_CAL_P1  (0x8e)
#define BME280_CAL_P2  (0x90)
#define BME280_CAL_P3  (0x92)
#define BME280_CAL_P4  (0x94)
#define BME280_CAL_P5  (0x96)
#define BME280_CAL_P6  (0x98)
#define BME280_CAL_P7  (0x9a)
#define BME280_CAL_P8  (0x9c)
#define BME280_CAL_P9  (0x9e)
#define BME280_CAL_H1  (0xa1) /* 8 bits */
#define BME280_CAL_H2  (0xe1)
#define BME280_CAL_H3  (0xe3) /* 8 bits */
#define BME280_CAL_H4  (0xe4) /* 12 bits, combined with H45 */
#define BME280_CAL_H45  (0xe5) /* 12 bits, combined with H5 */
#define BME280_CAL_H5  (0xe6) /* 8 bits */
#define BME280_CAL_H6  (0xe7) /* 8 bits */


// Control registers
#define BME280_ID_REGISTER  (0xd0) /* 8 bits */
#define BME280_RESET_REGISTER  (0xe0) /* 8 bits */
#define BME280_CTRL_HUM_REGISTER  (0xf2) /* 8 bits */
#define BME280_STATUS_REGISTER  (0xf3) /* 8 bits */
#define BME280_CTRL_MEAS_REGISTER  (0xf4) /* 8 bits */
#define BME280_CONFIG_REGISTER  (0xf5) /* 8 bits */


// Measurement registers.
#define BME280_PRESSURE  (0xf7) /* 20 bits */
#define BME280_PRESSURE_MSB  (0xf7) /* 8 bits */
#define BME280_PRESSURE_LSB  (0xf8) /* 8 bits */
#define BME280_PRESSURE_XLSB  (0xf9) /* 8 bits */
#define BME280_TEMPERATURE  (0xfa) /* 20 bits */
#define BME280_TEMPERATURE_MSB  (0xfa) /* 8 bits */
#define BME280_TEMPERATURE_LSB  (0xfb) /* 8 bits */
#define BME280_TEMPERATURE_XLSB  (0xfc) /* 8 bits */
#define BME280_HUMIDITY  (0xfd) /* 16 bits */
#define BME280_HUMIDITY_MSB  (0xfd) /* 8 bits */
#define BME280_HUMIDITY_LSB  (0xfe) /* 8 bits */




// It is recommended to read all the measurements in one go.
#define BME280_MEASUREMENT_REGISTER  (BME280_PRESSURE)
#define BME280_MEASUREMENT_SIZE  (8)


// Values for osrs_p & osrs_t fields of CTRL_MEAS register.
#define BME280_SKIP  (0)
#define BME280_OVERSAMPLING_1X  (1)
#define BME280_OVERSAMPLING_2X  (2)
#define BME280_OVERSAMPLING_4X  (3)
#define BME280_OVERSAMPLING_8X  (4)
#define BME280_OVERSAMPLING_16X  (5)


// Values for mode field of CTRL_MEAS register.
#define BME280_MODE_SLEEP  (0)
#define BME280_MODE_FORCED  (1)
#define BME280_MODE_NORMAL  (3)


// Value for RESET register.
#define BME280_RESET  (0xb6)


// Value of ID register.
#define BME280_ID  (uint8_t)(0x60)


// Values for t_sb field of CONFIG register
#define BME280_STANDBY_500_US  (0)
#define BME280_STANDBY_10_MS  (6)
#define BME280_STANDBY_20_MS  (7)
#define BME280_STANDBY_63_MS  (1)
#define BME280_STANDBY_125_MS  (2)
#define BME280_STANDBY_250_MS  (3)
#define BME280_STANDBY_500_MS  (4)
#define BME280_STANDBY_1000_MS  (5)


// Values for filter field of CONFIG register
#define BME280_FILTER_OFF  (0)
#define BME280_FILTER_COEFF_2  (1)
#define BME280_FILTER_COEFF_4  (2)
#define BME280_FILTER_COEFF_8  (3)
#define BME280_FILTER_COEFF_16  (4)


// I2C write function must support repeated start to avoid interruption of transactions.
// User-provided function to write data_size bytes from buffer p_data to I2C device at bus address i2c_address.
// Provide empty function if not used.
extern void i2cWrite(uint8_t i2c_address, uint8_t *p_data, uint8_t data_size, uint8_t repeated_start);
// User-provided function to read data_size bytes from I2C device at bus address i2c_address to buffer p_data.
// Provide empty function if not used.
extern void i2cRead(uint8_t i2c_address, uint8_t *p_data, uint8_t data_size);

// SPI functions must activate (make low) BME280 CSB pin before doing a transfer
// and deactivate it (make high) when done.
// User-provided function to write data_size bytes from buffer p_data to SPI device.
// Provide empty function if not used.
extern void spiWrite(uint8_t *p_data, uint8_t data_size);
// User-provided function to read data_size bytes from SPI device to buffer p_data.
// Provide empty function if not used.
extern void spiRead(uint8_t *p_data, uint8_t data_size);



class BME280 : public Device {
public:
    BME280();

    // Device methods
    std::vector<baro_reading_t> read();
    void printReadings(const std::vector<baro_reading_t>& readings);
    status checkOK() override;
    status init();
    uint8_t begin(uint8_t i2cAddress);
    uint8_t readId(void);
    void stop() override;

protected: 
    // void watchdog_task(void *parameters) override;
    // void watchdog_callback(TimerHandle_t xtimer) override;

private:
    // std::shared_ptr<idf::I2CAddress> addr;
    // std::shared_ptr<idf::I2CMaster> i2c;
    uint8_t _i2c_address; // address 
    void busWrite(uint8_t *p_data, uint8_t data_size, uint8_t repeated_start);
    void busRead(uint8_t *p_data, uint8_t data_size);
 

    // Calibration data.
    //--Temperature calibration
    uint16_t _dig_T1;
    int16_t _dig_T2;
    int16_t _dig_T3;
    //--Pressure calibration
    uint16_t _dig_P1;
    int16_t _dig_P2;
    int16_t _dig_P3;
    int16_t _dig_P4;
    int16_t _dig_P5;
    int16_t _dig_P6;
    int16_t _dig_P7;
    int16_t _dig_P8;
    int16_t _dig_P9;
    //--Humidity calibration
    uint8_t _dig_H1;
    int16_t _dig_H2;
    uint8_t _dig_H3;
    int16_t _dig_H4;
    int16_t _dig_H5;
    int8_t _dig_H6;
    void clearCalibrationData(void);
    
    
    // helpful stuff
    uint8_t readUint8(uint8_t reg);
    uint16_t readUint16(uint8_t reg);


    // The variable t_fine (signed 32 bit) carries a fine resolution temperature value over to the pressure and
    // humidity compensation formula and could be implemented as a global variable.
    int32_t _t_fine;
    temp_t _temperature;
    press_t _pressure;
    humid_t _humidity;  
};


#endif

