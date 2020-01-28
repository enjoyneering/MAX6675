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

   Board:                                     Level
   Uno, Mini, Pro, ATmega168, ATmega328.....  5v
   Mega, Mega2560, ATmega1280, ATmega2560...  5v
   Due, SAM3X8E.............................  3.3v
   Leonardo, ProMicro, ATmega32U4...........  5v
   Blue Pill, STM32F103xxxx boards..........  3v
   NodeMCU 1.0, WeMos D1 Mini...............  3v/5v*
   ESP32....................................  3v

                                              *most boards has 10-12kOhm pullup-up resistor on GPIO2/D4 & GPIO0/D3
                                               for flash & boot

   Frameworks & Libraries:
   ATtiny  Core          - https://github.com/SpenceKonde/ATTinyCore
   ESP32   Core          - https://github.com/espressif/arduino-esp32
   ESP8266 Core          - https://github.com/esp8266/Arduino
   STM32   Core          - https://github.com/stm32duino/Arduino_Core_STM32
                         - https://github.com/rogerclarkmelbourne/Arduino_STM32

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#ifndef MAX6675Soft_h
#define MAX6675Soft_h

/*
   Unfortunately, you cannot #define something in the sketch & get
   it in the library, because the Arduino toolchain includes library
   files & compiles them in advance, not knowing where it will be used.

   - uncomment to disable interrupts during bit-bang
*/
//#define MAX6675_DISABLE_INTERRUPTS

#define MAX6675_SOFT_SPI //disable upload hw driver spi.h

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
