/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== blink.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Display Header Files */
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "garmin.h"

I2C_Params garminParams;
I2C_Handle garminHandle;
I2C_Transaction garminTransaction = {0};

void garmin_Reset(uint16_t *distance);
void garmin_ReadData(int16_t accelerometer[3], int16_t gyro[3], int16_t *temp);

//void garmin_command(uint8_t command, uint8_t * writeData, size_t writeCount, uint8_t * readData, size_t readCount);

#define garmin_ADDRESS 0x62

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    //int16_t accelerometer[3], gyro[3], temp;
    uint16_t distance;

    /* Call driver init functions */
    //GPIO_init();
    //Display_init();
    I2C_init();

    // Initialize Display params

    Display_Params uartParams;
    Display_Handle uartHandle;
    Display_Params_init(&uartParams);
    uartHandle = Display_open(Display_Type_UART, &uartParams);

    // I2C Configuration
    I2C_Params_init(&garminParams);
    garminParams.bitRate = I2C_400kHz;
    garminParams.transferMode = I2C_MODE_BLOCKING;
    garminHandle = I2C_open(CONFIG_I2C_0, &garminParams);


    // Asynchronoous power mode needs to be enabled eventually
    // but we will default to always on for now to test high accuracy


    // This makes 0x50 measurements to be avgd before sending final output
/*
    uint8_t writeData[2] = {HIGH_ACCURACY_MODE, 0x80};
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 2;                      // How many bytes to send
    I2C_transfer(garminHandle, &garminTransaction);
*/
    turn_on(0x80);
    garmin_Reset(&distance);
    Display_print1(uartHandle, 0, 0, "Distance= %2f \r\n", ((float)distance)/100);
    turn_off();
    return 0;
}

/*
void garmin_command(uint8_t command, uint8_t * writeData, size_t writeCount, uint8_t * readData, size_t readCount) {
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = &command;                // What we send
    garminTransaction.writeCount = 1;                      // How many bytes to send
    I2C_transfer(garminHandle, &garminTransaction);

    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = writeCount;                      // How many bytes to send
    garminTransaction.readBuf = readData;                  // Where to store data
    garminTransaction.readCount = readCount;
    I2C_transfer(garminHandle, &garminTransaction);
}
*/
void garmin_Reset(uint16_t *distance) {
    uint8_t writeData[2] = {ACQ_COMMANDS, 0x04};
    uint8_t readData[2];
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 2;                      // How many bytes to send
    I2C_transfer(garminHandle, &garminTransaction);

    do {
    writeData[0] = STATUS;
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 1;                      // How many bytes to send
    garminTransaction.readBuf = readData;                  // Where to store data
    garminTransaction.readCount = 1;                       // How many bytes to read
    I2C_transfer(garminHandle, &garminTransaction);
    } while(readData[0] & 0x1 == 1);

    writeData[0] = 0x10;
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 1;                      // How many bytes to send
    garminTransaction.readBuf = readData;                  // Where to store data
    garminTransaction.readCount = 2;                       // How many bytes to read
    I2C_transfer(garminHandle, &garminTransaction);

    *distance = (readData[1] << 8) | readData[0];
}

void turn_on (uint8_t accuracy) {
    // turns on always on mode and turns on high accuracy mode
    uint8_t writeData0[2] = {POWER_MODE, 0xFF};
    garminTransaction.slaveAddress = garmin_ADDRESS;
    garminTransaction.writeBuf = writeData0;
    garminTransaction.writeCount = 2;
    I2C_transfer(garminHandle, &garminTransaction);

    uint8_t writeData1[2] = {HIGH_ACCURACY_MODE, accuracy};
    garminTransaction.slaveAddress = garmin_ADDRESS;
    garminTransaction.writeBuf = writeData1;
    garminTransaction.writeCount = 2;
    I2C_transfer(garminHandle, &garminTransaction);
}

void turn_off (void) {
    // turn off high accuracy mode and turns on asynchronous mode
    uint8_t writeData0[2] = {HIGH_ACCURACY_MODE, 0x00};
    garminTransaction.slaveAddress = garmin_ADDRESS;
    garminTransaction.writeBuf = writeData0;
    garminTransaction.writeCount = 2;
    I2C_transfer(garminHandle, &garminTransaction);

    uint8_t writeData1[2] = {POWER_MODE, 0x00};
    garminTransaction.slaveAddress = garmin_ADDRESS;
    garminTransaction.writeBuf = writeData1;
    garminTransaction.writeCount = 2;
    I2C_transfer(garminHandle, &garminTransaction);
}


void garmin_ReadData(int16_t accelerometer[3], int16_t gyro[3], int16_t *temp) {
    uint8_t writeData[1];
    uint8_t readData[6];

    // read data from accelerometer
    writeData[0] = 0x3B;
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 1;                      // How many bytes to send
    garminTransaction.readBuf = readData;                  // Where to store data
    garminTransaction.readCount = 6;                       // How many bytes to read
    I2C_transfer(garminHandle, &garminTransaction);

    accelerometer[0] = ((readData[0] << 8) | readData[1]); // x-axis data
    accelerometer[1] = ((readData[2] << 8) | readData[3]); // y-axis data
    accelerometer[2] = ((readData[4] << 8) | readData[5]); // z-axis data

    // read data from gyro
    writeData[0] = 0x43;
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 1;                      // How many bytes to send
    garminTransaction.readBuf = readData;                  // Where to store data
    garminTransaction.readCount = 6;                       // How many bytes to read
    I2C_transfer(garminHandle, &garminTransaction);

    gyro[0] = ((readData[0] << 8) | readData[1]); // x-axis data
    gyro[1] = ((readData[2] << 8) | readData[3]); // y-axis data
    gyro[2] = ((readData[4] << 8) | readData[5]); // z-axis data

    // read chip temperature
    writeData[0] = 0x41;
    garminTransaction.slaveAddress = garmin_ADDRESS;      // Where we send to
    garminTransaction.writeBuf = writeData;                // What we send
    garminTransaction.writeCount = 1;                      // How many bytes to send
    garminTransaction.readBuf = readData;                  // Where to store data
    garminTransaction.readCount = 2;                       // How many bytes to read
    I2C_transfer(garminHandle, &garminTransaction);

    *temp = ((readData[0] << 8) | readData[1]);             // temp data
}
