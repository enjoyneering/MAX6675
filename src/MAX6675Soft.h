/***************************************************************************************************/
/*
   This is an Arduino library for 12-bit MAX6675 K-Thermocouple to Digital Converter
   with Cold Junction Compensation conneted to software/bit-bang SPI with maximum sampling
   rate ~4..5Hz.

   - MAX6675 power supply voltage is 3.0 - 5.5v
   - K-type thermocouples have an absolute accuracy of around ±2°C
   - Measurement tempereture range 0°C...+1024°C with 0.25°C resolution
   - Cold junction compensation range -20°C...+85°C
   - Keep K-type thermocouple cold junction & MAX6675 at the same temperature
   - Avoid placing heat-generating devices or components near the converter
     because this may produce errors
   - It is strongly recommended to add a 10nF/0.01mF ceramic surface-mount capacitor, placed across
     the T+ and T- pins, to filter noise on the thermocouple lines.
     
   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/MAX6675

   This sensor uses SPI bus to communicate, specials pins are required to interface
   Board:                                    MOSI        MISO        SCLK         SS, don't use for CS   Level
   Uno, Mini, Pro, ATmega168, ATmega328..... 11          12          13           10                     5v
   Mega, Mega2560, ATmega1280, ATmega2560... 51          50          52           53                     5v
   Due, SAM3X8E............................. ICSP4       ICSP1       ICSP3        x                      3.3v
   Leonardo, ProMicro, ATmega32U4........... 16          14          15           x                      5v
   Blue Pill, STM32F103xxxx boards.......... PA17        PA6         PA5          PA4                    3v
   NodeMCU 1.0, WeMos D1 Mini............... GPIO13/D7   GPIO12/D6   GPIO14/D5    GPIO15/D8*             3v/5v
   ESP32.................................... GPIO23/D23  GPIO19/D19  GPIO18/D18   x                      3v

                                            *if GPIO2/D4 or GPIO0/D3 used for for CS, apply an external 25kOhm
                                             pullup-down resistor

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#ifndef MAX6675Soft_h
#define MAX6675Soft_h

#define MAX6675_SOFT_SPI //disable upload spi.h

#include <MAX6675.h>


class MAX6675Soft : public MAX6675
{
  public:
   MAX6675Soft(uint8_t cs, uint8_t so, uint8_t sck);

   void     begin(void);
   uint16_t readRawData(void);
 

  private:
   uint8_t _so;
   uint8_t _sck;
};

#endif
