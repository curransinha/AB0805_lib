// I2Cdev library collection - AB0805 I2C device class
// Based on Abracon AB08X5 Real-Time Clock Family datasheet, 2015
// 03/07/2015 by Curran Sinha (curransinha@gmail.com)
// Based heavily off of Jeff Rowberg's DS1307 RTC Library
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2015-03-09 - initial release

/* ============================================ 
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "AB0805.h"

/** Default constructor, uses default I2C address.
 * @see AB0805_DEFAULT_ADDRESS
 */
AB0805::AB0805() {
    devAddr = AB0805_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see AB0805_DEFAULT_ADDRESS
 * @see AB0805_ADDRESS
 */
AB0805::AB0805(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * Stops clock. Intialized to use crystal oscillator unless one is not found
 * then the RC oscillator is used.
 */
void AB0805::initialize() {
    uint8_t stopClk = 0x91;
    uint8_t allowOscEdit = 0xA1;
    uint8_t xtOscSel = 0x08;
    I2Cdev::writeByte(devAddr, AB0805_RA_CONTROL1, stopClk);      //stops clock
    I2Cdev::writeByte(devAddr, AB0805_RA_CONFIG_KEY, allowOscEdit);  //Allows edits to osc. cntrl register (0x1C)
    I2Cdev::writeByte(devAddr, AB0805_RA_OSC_CONTROL, xtOscSel);   //Crystal used (switch to RC if XT osc failure)
    
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool AB0805::testConnection() {
    if (I2Cdev::readByte(devAddr, AB0805_RA_ID0 , buffer) == 0x08) {
        return true;
    }
    return false;
}

void AB0805::startClock() {
    I2Cdev::writeBit(devAddr, AB0805_RA_CONTROL1, AB0805_CONTROL1_STOP_BIT, 0);
    }

void AB0805::stopClock() {
    I2Cdev::writeBit(devAddr, AB0805_RA_CONTROL1, AB0805_CONTROL1_STOP_BIT, 1);
    }
void AB0805::useRcOsc() {
    uint8_t allowOscEdit = 0xA1;
    I2Cdev::writeByte(devAddr, AB0805_RA_CONFIG_KEY, allowOscEdit);  //Allows edits to osc. cntrl register (0x1C)
    I2Cdev::writeBit(devAddr, AB0805_RA_OSC_CONTROL, AB0805_OSC_CONTROL_OSC_SEL, 1);
    }

// HUNDREDTHS register -- only valid with XT oscillator   
uint16_t AB0805::getHundredths() {
    I2Cdev::readByte(devAddr, AB0805_RA_HUNDREDTHS, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0xF0) >> 4) * 10;
}
void AB0805::setHundredths(uint16_t hundredths) {
    if (hundredths > 99) return;
    uint8_t value = ((hundredths/ 10) << 4) + (hundredths % 10);
    I2Cdev::writeByte(devAddr, AB0805_RA_HUNDREDTHS, value);
}

// SECONDS register
uint8_t AB0805::getSeconds() {
    // Byte: [7 = CH] [6:4 = 10SEC] [3:0 = 1SEC]
    I2Cdev::readByte(devAddr, AB0805_RA_SECONDS, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}
void AB0805::setSeconds(uint8_t seconds) {
    if (seconds > 59) return;
    uint8_t value = 0x00 + ((seconds / 10) << 4) + (seconds % 10);
    I2Cdev::writeByte(devAddr, AB0805_RA_SECONDS, value);
}

// MINUTES register
uint8_t AB0805::getMinutes() {
    // Byte: [7 = 0] [6:4 = 10MIN] [3:0 = 1MIN]
    I2Cdev::readByte(devAddr, AB0805_RA_MINUTES, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;

}
void AB0805::setMinutes(uint8_t minutes) {
    if (minutes > 59) return;
    uint8_t value = ((minutes / 10) << 4) + (minutes % 10);
    I2Cdev::writeByte(devAddr, AB0805_RA_MINUTES, value);
}

// HOURS register
uint8_t AB0805::getMode() {
    I2Cdev::readBit(devAddr, AB0805_RA_CONTROL1, AB0805_CONTROL1_12OR24_BIT, buffer);
    mode12 = buffer[0];
    return buffer[0];
}
void AB0805::setMode(uint8_t mode) {
    I2Cdev::writeBit(devAddr, AB0805_RA_CONTROL1, AB0805_CONTROL1_12OR24_BIT, mode);
}
uint8_t AB0805::getAMPM() {
    I2Cdev::readBit(devAddr, AB0805_RA_HOURS, AB0805_HOURS_AMPM_BIT, buffer);
    return buffer[0];
}
//0 = AM Hours, 1 = PM Hours
void AB0805::setAMPM(uint8_t ampm) {
    I2Cdev::writeBit(devAddr, AB0805_RA_HOURS, AB0805_HOURS_AMPM_BIT, ampm);
}
uint8_t AB0805::getHours12() {
    I2Cdev::readByte(devAddr, AB0805_RA_HOURS, buffer);
    mode12 = getMode();
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        uint8_t hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;

        // convert 24-hour to 12-hour format, since that's what's requested
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        return hours;
    }
}
void AB0805::setHours12(uint8_t hours, uint8_t ampm) {
    if (hours > 12 || hours < 1) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) ampm = 0x20;
        uint8_t value = ampm + ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, AB0805_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) hours += 12;
        if (hours == 0) hours = 12; // 12 AM
        else if (hours == 24) hours = 12; // 12 PM, after +12 adjustment
        uint8_t value = ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, AB0805_RA_HOURS, value);
    }
}
uint8_t AB0805::getHours24() {
    I2Cdev::readByte(devAddr, AB0805_RA_HOURS, buffer);
    mode12 = getMode();
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        uint8_t hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;

        // convert 12-hour to 24-hour format, since that's what's requested
        if (buffer[0] & 0x20) {
            // currently PM
            if (hours < 12) hours += 12;
        } else {
            // currently AM
            if (hours == 12) hours = 0;
        }
        return hours;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
    }
}
void AB0805::setHours24(uint8_t hours) {
    if (hours > 23) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        uint8_t ampm = 0;
        if (hours > 11) ampm = 0x20;
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        uint8_t value = ampm + ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, AB0805_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        uint8_t value = ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, AB0805_RA_HOURS, value);
    }
}

// DAY register
uint8_t AB0805::getDayOfWeek() {
    I2Cdev::readBits(devAddr, AB0805_RA_DAY, AB0805_DAY_BIT, AB0805_DAY_LENGTH, buffer);
    return buffer[0];
}
void AB0805::setDayOfWeek(uint8_t dow) {
    if (dow < 1 || dow > 7) return;
    I2Cdev::writeBits(devAddr, AB0805_RA_DAY, AB0805_DAY_BIT, AB0805_DAY_LENGTH, dow);
}

// DATE register
uint8_t AB0805::getDay() {
    // Byte: [7:6 = 0] [5:4 = 10DAY] [3:0 = 1DAY]
    I2Cdev::readByte(devAddr, AB0805_RA_DATE, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
}
void AB0805::setDay(uint8_t day) {
    uint8_t value = ((day / 10) << 4) + (day % 10);
    I2Cdev::writeByte(devAddr, AB0805_RA_DATE, value);
}

// MONTH register
uint8_t AB0805::getMonth() {
    // Byte: [7:5 = 0] [4 = 10MONTH] [3:0 = 1MONTH]
    I2Cdev::readByte(devAddr, AB0805_RA_MONTH, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
}
void AB0805::setMonth(uint8_t month) {
    if (month < 1 || month > 12) return;
    uint8_t value = ((month / 10) << 4) + (month % 10);
    I2Cdev::writeByte(devAddr, AB0805_RA_MONTH, value);
}

// YEAR register
uint16_t AB0805::getYear() {
    I2Cdev::readByte(devAddr, AB0805_RA_YEAR, buffer);
    return 2000 + (buffer[0] & 0x0F) + ((buffer[0] & 0xF0) >> 4) * 10;
}
void AB0805::setYear(uint16_t year) {
    if (year < 2000) return;
    year -= 2000;
    uint8_t value = ((year / 10) << 4) + (year % 10);
    I2Cdev::writeByte(devAddr, AB0805_RA_YEAR, value);
}


// convenience methods

void AB0805::getDate(uint16_t *year, uint8_t *month, uint8_t *day) {
    *year = getYear();
    *month = getMonth();
    *day = getDay();
}
void AB0805::setDate(uint16_t year, uint8_t month, uint8_t day) {
    setYear(year);
    setMonth(month);
    setDay(day);
}

void AB0805::getTime12(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm) {
    *hours = getHours12();
    *minutes = getMinutes();
    *seconds = getSeconds();
    *ampm = getAMPM();
}
void AB0805::setTime12(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm) {
    setSeconds(seconds);
    setMinutes(minutes);
    setHours12(hours, ampm);
}

void AB0805::getTime24(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    *hours = getHours24();
    *minutes = getMinutes();
    *seconds = getSeconds();
}
void AB0805::setTime24(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    setSeconds(seconds);
    setMinutes(minutes);
    setHours24(hours);
}

void AB0805::getDateTime12(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm) {
    getTime12(hours, minutes, seconds, ampm);
    getDate(year, month, day);
}
void AB0805::setDateTime12(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm) {
    setTime12(hours, minutes, seconds, ampm);
    setDate(year, month, day);
}

void AB0805::getDateTime24(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    getTime24(hours, minutes, seconds);
    getDate(year, month, day);
}
void AB0805::setDateTime24(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    setTime24(hours, minutes, seconds);
    setDate(year, month, day);
}

