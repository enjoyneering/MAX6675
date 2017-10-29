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

#include <MAX6675.h>


/**************************************************************************/
/*
    MAX6675()

    Constructor

    NOTE:
    cs  - chip select, set CS low to enable the serial interface
    so  - serial data output
    sck - serial clock input
*/
/**************************************************************************/
MAX6675::MAX6675(uint8_t cs, uint8_t so, uint8_t sck)
{
  _cs    = cs;
  _so    = so;
  _sck   = sck;
}

/**************************************************************************/
/*
    begin()

    Initializes & configures I2C
*/
/**************************************************************************/
void MAX6675::begin(void)
{
  pinMode(_cs, OUTPUT);
  pinMode(_so, INPUT);
  pinMode(_sck, OUTPUT);

  digitalWrite(_cs, HIGH); //disables the serial interface & start new measurement process
  digitalWrite(_sck, LOW);
}

/**************************************************************************/
/*
    detectThermocouple()

    Checks if thermocouple is broken or unplugged, returns true if connected

    NOTE:
    - bit D2 from 16-bits is normally low & goes high when the thermocouple
      is broken or unplugged
    - to allow the operation of the open thermocouple detector the
      "T-" pin must be grounded!!!
*/
/**************************************************************************/
bool MAX6675::detectThermocouple(uint16_t rawValue)
{
  return ~bitRead(rawValue, 2);
}

/**************************************************************************/
/*
    getChipID()

    Checks chip ID

    NOTE:
    - bit D1 is low to provide a device ID for the MAX6675.
*/
/**************************************************************************/
uint16_t MAX6675::getChipID(uint16_t rawValue)
{
  if (bitRead(rawValue, 1) == 0) return 6675;
  return 0;
}

/**************************************************************************/
/*
    getTemperature()

    Reads Temperature, C

    NOTE:
    - bits D14..D3 contain the converted temperature in the order of MSB to LSB.
    - sequence of all zeros on SO pin means the thermocouple reading is 0C
      sequence of all ones  on SO pin means the thermocouple reading is +1023.75C
*/
/**************************************************************************/
float MAX6675::getTemperature(uint16_t rawValue)
{
  if (detectThermocouple(rawValue) != true || getChipID(rawValue) != 6675) return MAX6675_ERROR;

  rawValue = rawValue >> 3; //clear D2..D0 bits, rawValue - 0..4095

  return (float)rawValue * MAX6675_RESOLUTION;
}

/**************************************************************************/
/*
    readRawData()

    Reads raw data from MAX6675, bit-banging implementation

    NOTE:
    - forcing CS low immediately stops any conversion process, force CS high
      to initiate a new measurement process
    - set CS low to enable the serial interface & force to output the first bit on the SO pin,
      apply 16 clock signals at SCK to read the results at SO on the falling edge of the SCK
    - bit D15 is a dummy sign bit and is always zero.
    - bits D14..D3 contain the converted temperature in the order of MSB to LSB
    - bit D2 is normally low & goes high when the thermocouple is broken or unplugged
      to allow the operation of the open thermocouple detector the "T-" pin must be grounded
    - bit D1 is low to provide a device ID for the MAX6675
    - bit D0 is three-state
    - sequence of all zeros on SO pin means the thermocouple reading is 0C
      sequence of all ones  on SO pin means the thermocouple reading is +1023.75C
*/
/**************************************************************************/
uint16_t MAX6675::readRawData(void)
{
  uint16_t rawData = 0;

  /* stops conversion & initiate a new measurement process */
  digitalWrite(_cs, LOW);         //stop  conversion
  digitalWrite(_cs, HIGH);        //start conversion
  delay(MAX6675_CONVERSION_TIME);

  /* set CS low to enable serial interface, read 16-bits, in order MSB->LSB (15..0bit) */
  digitalWrite(_cs, LOW);

  for (int8_t i = 16; i >= 0; i--)
  {
    digitalWrite(_sck, HIGH);
    rawData = (rawData << 1) | digitalRead(_so);
    digitalWrite(_sck, LOW);
  }

  /* disables the serial interface & initiate measurement */
  digitalWrite(_cs, HIGH);

  return rawData;
}
