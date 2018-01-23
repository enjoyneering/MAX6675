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

#include <MAX6675.h>


/**************************************************************************/
/*
    MAX6675()

    Constructor for software/bit-bang serial interface

    NOTE:
    cs  - chip select, set CS low to enable the serial interface
    so  - serial data output
    sck - serial clock input
*/
/**************************************************************************/
MAX6675::MAX6675(uint8_t cs, uint8_t so, uint8_t sck)
{
  _useHardwareSPI = false; //false ->sw spi

  _cs  = cs;               //sw ss
  _so  = so;               //sw miso
  _sck = sck;              //sw sclk
}

/**************************************************************************/
/*
    MAX6675()

    Constructor for hardware SPI serial interface

    NOTE:
    - chip select "cs", set CS low to enable the serial interface
*/
/**************************************************************************/
MAX6675::MAX6675(uint8_t cs)
{
  _useHardwareSPI = true; //true -> hw spi

  _cs  = cs;              //hw ss
  _so  = 0;               //sw miso
  _sck = 0;               //sw sclk
}

/**************************************************************************/
/*
    begin()

    Initializes & configures gpio for bit-bang or hardware serial interface

    NOTE:

*/
/**************************************************************************/
void MAX6675::begin(void)
{
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);     //disables the serial interface, but it will initiate measurement/conversion

  switch (_useHardwareSPI)     //true -> hw spi, false ->sw spi
  {
    case true:
      SPI.begin();             //setting hardware SCK, MOSI, SS to output, pull SCK, MOSI low & SS high      
      break;

    case false:
      pinMode(_so, INPUT);
      pinMode(_sck, OUTPUT);
      digitalWrite(_sck, LOW);     
      break;
  }
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
  if (rawValue == MAX6675_FORCE_READ_DATA) rawValue = readRawData();

  if (bitRead(rawValue, 1) == 0) return MAX6675_ID;

  return 0;
}

/**************************************************************************/
/*
    getTemperature()

    Reads Temperature, °C

    NOTE:
    - bits D14..D3 contain the converted temperature in the order of MSB to LSB.
    - sequence of all zeros on SO pin means the thermocouple reading is 0°C
    - sequence of all ones  on SO pin means the thermocouple reading is +1023.75°C
*/
/**************************************************************************/
float MAX6675::getTemperature(uint16_t rawValue)
{
  if (rawValue == MAX6675_FORCE_READ_DATA) rawValue = readRawData();

  if (detectThermocouple(rawValue) != true || getChipID(rawValue) != MAX6675_ID) return MAX6675_ERROR;

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
    - sequence of all zeros on SO pin means the thermocouple reading is 0°C
    - sequence of all ones  on SO pin means the thermocouple reading is +1023.75°C

    - max SPI master clock speed is equal with board speed
      (16000000UL for 5V 16MHz/ProMini), but MAX6675 max speed is only 4.3MHz
    - SPI_MODE0 -> capture data on clock's falling edge.
*/
/**************************************************************************/
uint16_t MAX6675::readRawData(void)
{
  uint16_t rawData = 0;

  digitalWrite(_cs, LOW);                                                //stop  measurement/conversion
  delay(1);                                                              //4MHz  is 0.25usec, do we need it???
  digitalWrite(_cs, HIGH);                                               //start measurement/conversion
  delay(MAX6675_CONVERSION_TIME);

  digitalWrite(_cs, LOW);                                                //set CS low to enable serial interface

  switch (_useHardwareSPI)                                               //true -> hw spi, false ->sw spi
  {
    case true:
      SPI.beginTransaction(SPISettings(4000000UL, MSBFIRST, SPI_MODE0)); //speed 4MHz, read msb first, spi mode 0, see note
      rawData = SPI.transfer16(0x0000);                                  //MAX6675 has read only spi & mosi not connected, doesn't metter what to send
      break;

    case false:
      for (uint8_t i = 0; i < 16; i++)                                   //read 16-bits via software spi, in order MSB->LSB (D15..D0 bit)
      {
        digitalWrite(_sck, HIGH);
        rawData = (rawData << 1) | digitalRead(_so);
        digitalWrite(_sck, LOW);
      }
      break;
  }  

  digitalWrite(_cs, HIGH);                                               //disables the serial interface, but it will initiate measurement/conversion

  if (_useHardwareSPI == true) SPI.endTransaction();                     //call to de-asserting hw chip select & free hw spi for other slaves

  return rawData;
}
