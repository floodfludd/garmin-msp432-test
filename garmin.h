/*
 * garmin.h
 *
 *  Created on: Feb 21, 2022
 *      Author: chrom
 */

#ifndef GARMIN_H_
#define GARMIN_H_


enum
  {
    ACQ_COMMANDS = 0x00,
    STATUS = 0x01,
    ACQUISITION_COUNT = 0x05,
    FULL_DELAY_LOW = 0x10,
    FULL_DELAY_HIGH = 0x11,
    UNIT_ID_0 = 0x16,
    UNIT_ID_1 = 0x17,
    UNIT_ID_2 = 0x18,
    UNIT_ID_3 = 0x19,
    I2C_SEC_ADDR = 0x1A,
    I2C_CONFIG = 0x1B,
    DETECTION_SENSITIVITY = 0x1C,
    LIB_VERSION = 0x30,
    CORR_DATA = 0x52,
    CP_VER_LO = 0x72,
    CP_VER_HI = 0x73,
    BOARD_TEMPERATURE = 0xE0,
    HARDWARE_VERSION = 0xE1,
    POWER_MODE = 0xE2,
    MEASUREMENT_INTERVAL = 0xE3,
    FACTORY_RESET = 0xE4,
    QUICK_TERMINATION = 0xE5,
    START_BOOTLOADER = 0xE6,
    ENABLE_FLASH_STORAGE = 0xEA,
    HIGH_ACCURACY_MODE = 0xEB,
    SOC_TEMPERATURE = 0xEC,
    ENABLE_ANT_RADIO = 0xF0,
  };

    void turn_on (uint8_t accuracy);
    void turn_off (void);

#endif /* GARMIN_H_ */
