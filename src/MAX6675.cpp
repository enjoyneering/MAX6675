/***************************************************************************************************/
/*
   This is an Arduino library for 12-bit MAX6675 K-Thermocouple to Digital Converter
   with Cold Junction Compensation conneted to hardware 4Mhz SPI with maximum sampling
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

#include <MAX6675.h>

/**************************************************************************/
/*
    MAX6675()

    Constructor for hardware read only SPI

    NOTE:
    - cs is chip select, set CS low to enable the serial interface
*/
/**************************************************************************/
MAX6675::MAX6675(uint8_t cs)
{
  _cs = cs; //cs chip select
}

/**************************************************************************/
/*
    begin()

    Initializes & configures hardware SPI
*/
/**************************************************************************/
void MAX6675::begin(void)
{
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH); //disables SPI interface for MAX6675, but it will initiate measurement/conversion

  SPI.begin();             //setting hardware SCK, MOSI, SS to output, pull SCK, MOSI low & SS high      
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
    - SPI_MODE0 -> capture data on clock's falling edge
*/
/**************************************************************************/
uint16_t MAX6675::readRawData(void)
{
  uint16_t rawData = 0;

  digitalWrite(_cs, LOW);                                            //stop  measurement/conversion
  delayMicroseconds(1);                                              //4MHz  is 0.25usec, do we need it???
  digitalWrite(_cs, HIGH);                                           //start measurement/conversion
  delay(MAX6675_CONVERSION_TIME);

  digitalWrite(_cs, LOW);                                            //set CS low to enable SPI interface for MAX6675

  SPI.beginTransaction(SPISettings(4000000UL, MSBFIRST, SPI_MODE0)); //speed ~4MHz, read MSB first, SPI mode 0, see note
  rawData = SPI.transfer16(0x0000);                                  //chip has read only SPI & MOSI not connected, so it doesn't metter what to send
 
  digitalWrite(_cs, HIGH);                                           //disables SPI interface for MAX6675, but it will initiate measurement/conversion

  SPI.endTransaction();                                              //de-asserting hw chip select & free hw SPI for other slaves

  return rawData;
}
