// I2Cdev library collection - AB0805 I2C device class header file
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

#ifndef _AB0805_H_
#define _AB0805_H_

#include "I2Cdev.h"


#define AB0805_ADDRESS              0x69 // this device only has one address
#define AB0805_DEFAULT_ADDRESS      0x69

#define AB0805_RA_HUNDREDTHS        0X00
#define AB0805_RA_SECONDS           0x01
#define AB0805_RA_MINUTES           0x02
#define AB0805_RA_HOURS             0x03
#define AB0805_RA_DATE              0x04
#define AB0805_RA_MONTH             0x05
#define AB0805_RA_YEAR              0x06
#define AB0805_RA_DAY               0X07

#define AB0805_RA_HUNDREDTHS_ALARM  0X08
#define AB0805_RA_SECONDS_ALARM     0X09
#define AB0805_RA_MINUTES_ALARM     0X0A
#define AB0805_RA_HOURS_ALARM       0X0B
#define AB0805_RA_DATE_ALARM        0X0C
#define AB0805_RA_MONTH_ALARM       0X0D
#define AB0805_RA_WEEKDAYS_ALARM    0X0E

#define AB0805_RA_STATUS            0X0F
#define AB0805_RA_CONTROL1          0x10            
#define AB0805_RA_CONTROL2          0x11            //interrupt control
#define AB0805_RA_OSC_CONTROL       0x1C
#define AB0805_RA_OSC_STATUS        0x1D
//Config Key-written with specific values to access certain registers
//access Oscillator control (0x1C) -> write 0xA1
//Software Reset (doesn't update Config Key) -> write 0x3C
#define AB0805_RA_CONFIG_KEY           0X1F
#define AB0805_RA_ID0                  0X28            //0x08
#define AB0805_RA_ID1                  0X29            //0x05


#define AB0805_HUNDRETHS_10_BIT       7
#define AB0805_HUNDRETHS_10_LENGTH    4
#define AB0805_HUNDRETHS_1_BIT        3
#define AB0805_HUNDRETHS_1_LENGTH     4

#define AB0805_SECONDS_10_BIT         6
#define AB0805_SECONDS_10_LENGTH      3
#define AB0805_SECONDS_1_BIT          3
#define AB0805_SECONDS_1_LENGTH       4

#define AB0805_MINUTES_10_BIT         6
#define AB0805_MINUTES_10_LENGTH      3
#define AB0805_MINUTES_1_BIT          3
#define AB0805_MINUTES_1_LENGTH       4

//24 HOUR MODE if 0x10 bit 6 = 0
//12 HOUR MODE if 0X10 bit 6 = 1
#define AB0805_HOURS_AMPM_BIT         5         //2nd HOURS_10 bit if in 24-hour mode
#define AB0805_HOURS_10_BIT           4
#define AB0805_HOURS_1_BIT            3
#define AB0805_HOURS_1_LENGTH         4

#define AB0805_DATE_10_BIT            5
#define AB0805_DATE_10_LENGTH         2
#define AB0805_DATE_1_BIT             3
#define AB0805_DATE_1_LENGTH          4

#define AB0805_MONTH_10_BIT           4
#define AB0805_MONTH_1_BIT            3
#define AB0805_MONTH_1_LENGTH         4

#define AB0805_YEAR_10H_BIT           7
#define AB0805_YEAR_10H_LENGTH        4
#define AB0805_YEAR_1H_BIT            3
#define AB0805_YEAR_1H_LENGTH         4

#define AB0805_DAY_BIT                2
#define AB0805_DAY_LENGTH             3

#define AB0805_CONTROL1_STOP_BIT      7           //If 1 stops clocking system
#define AB0805_CONTROL1_12OR24_BIT    6           //0 -> 24 hour mode, 1-> 12 hour mode
#define AB0805_CONTROL_WRTC           0           //must be 1 to write to time registers
#define AB0805_OSC_CONTROL_OSC_SEL    7


class AB0805 {
    public:
        AB0805();
        AB0805(uint8_t address);

        void initialize();
        bool testConnection();

        void startClock();
        void stopClock();
        void useRcOsc();

        // HUNDREDTHS register - only valid with XT oscillator
        uint16_t getHundredths(); // 0-99
        void setHundredths(uint16_t hundredths);

        // SECONDS register
        uint8_t getSeconds(); // 0-59
        void setSeconds(uint8_t seconds);

        // MINUTES register
        uint8_t getMinutes(); // 0-59
        void setMinutes(uint8_t minutes);

        // HOURS register
        uint8_t getMode(); // 0-1
        void setMode(uint8_t mode);
        uint8_t getAMPM(); // 0-1
        void setAMPM(uint8_t ampm);
        uint8_t getHours12(); // 1-12
        void setHours12(uint8_t hours, uint8_t ampm);
        uint8_t getHours24(); // 0-23
        void setHours24(uint8_t hours);

        // DAY register
        uint8_t getDayOfWeek(); // 1-7
        void setDayOfWeek(uint8_t dow);

        // DATE register
        uint8_t getDay(); // 1-31
        void setDay(uint8_t day);
        
        // MONTH register
        uint8_t getMonth(); // 1-12
        void setMonth(uint8_t month);

        // YEAR register
        uint16_t getYear(); // 1970, 2000, 2011, etc
        void setYear(uint16_t year);
        

        // convenience methods

        void getDate(uint16_t *year, uint8_t *month, uint8_t *day);
        void setDate(uint16_t year, uint8_t month, uint8_t day);

        void getTime12(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm);
        void setTime12(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm);
        
        void getTime24(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
        void setTime24(uint8_t hours, uint8_t minutes, uint8_t seconds);
        
        void getDateTime12(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm);
        void setDateTime12(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm);
        
        void getDateTime24(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
        void setDateTime24(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);
        

    private:
        uint8_t devAddr;
        uint8_t buffer[1];
        bool mode12;
        
};

#endif /* _AB0805_H_ */