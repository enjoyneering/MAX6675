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

   BSD license, all text above must be included in any redistribution
*/
/***************************************************************************************************/

#ifndef MAX6675_h
#define MAX6675_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined (__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#define MAX6675_CONVERSION_TIME 220  //milliseconds or sampling rate ~4...5Hz
#define MAX6675_RESOLUTION      0.25 //deg.C per dac step
#define MAX6675_ERROR          -255  //if any error

class MAX6675
{
  public:
   MAX6675(uint8_t cs, uint8_t so, uint8_t sck);

   void     begin(void);
   bool     detectThermocouple(uint16_t rawValue);
   uint16_t getChipID(uint16_t rawValue);
   float    getTemperature(uint16_t rawValue);
   uint16_t readRawData(void);
 

  private:
   uint8_t _cs;
   uint8_t _so;
   uint8_t _sck;
};

#endif
