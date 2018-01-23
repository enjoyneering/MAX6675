/***************************************************************************************************/
/*
   This is an Arduino library for 12-bit MAX6675 K-Thermocouple to Digital Converter
   with Cold Junction Compensation & maximum sampling rate ~4...5Hz

   - K-type thermocouples have an absolute accuracy of around ±2°C
   - Measurement tempereture range 0°C...+1024°C with 0.25°C resolution
   - Cold junction compensation range -20°C...+85°C
   - Keep k-type thermocouple cold junction & MAX6675 at the same temperature
   - Avoid placing heat-generating devices or components near the converter
     because this may produce errors
     
   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/MAX6675

   This sensor can work with hardware SPI bus, specials pins are required to interface

   Connect chip to pins:    SCLK        MOSI        don't use SS for CS  don't use MISO for CS
   Uno, Mini, Pro:          13          12          10                   11
   Mega2560, Due:           52          51          53                   50
   Leonardo, ProMicro:      15          16          x                    14
   NodeMCU 1.0:             GPIO14/D5   GPIO13/D7   GPIO15/D8            GPIO12/D6
   WeMos D1 Mini:           GPIO14/D5   GPIO13/D7   GPIO15/D8            GPIO12/D6 

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#ifndef MAX6675_h
#define MAX6675_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SPI.h>

#if defined (__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#define MAX6675_CONVERSION_TIME 220    //in milliseconds, sampling rate ~4...5Hz
#define MAX6675_RESOLUTION      0.25   //in deg.C per dac step

#define MAX6675_ID              6675
#define MAX6675_FORCE_READ_DATA 0x8002 //force to read the data, is unique because d15d1 can't be high
#define MAX6675_ERROR           2000   //returned value if any error happends

class MAX6675
{
  public:
   MAX6675(uint8_t cs, uint8_t so, uint8_t sck);
   MAX6675(uint8_t cs);

   void     begin(void);
   bool     detectThermocouple(uint16_t rawValue = MAX6675_FORCE_READ_DATA);
   uint16_t getChipID(uint16_t rawValue = MAX6675_FORCE_READ_DATA);
   float    getTemperature(uint16_t rawValue = MAX6675_FORCE_READ_DATA);
   uint16_t readRawData(void);
 

  private:
   bool    _useHardwareSPI;
   uint8_t _cs;
   uint8_t _so;
   uint8_t _sck;
};

#endif
