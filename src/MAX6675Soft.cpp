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

                                             *if GPIO2/D4 or GPIO0/D3 is used, apply an external
                                              25kOhm pullup-down resistor otherwise reset & reset button
                                              may not work

   Frameworks & Libraries:
   ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
   ESP32 Core            - https://github.com/espressif/arduino-esp32
   ESP8266 Core          - https://github.com/esp8266/Arduino
   STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#include <MAX6675Soft.h>


/**************************************************************************/
/*
    MAX6675()

    Constructor for software/bit-bang read only SPI

    NOTE:
    cs  - chip select, set CS low to enable the serial interface
    so  - serial data output
    sck - serial clock input
*/
/**************************************************************************/
MAX6675Soft::MAX6675Soft(uint8_t cs, uint8_t so, uint8_t sck) : MAX6675(cs)
{
  _so  = so;  //sw miso
  _sck = sck; //sw sclk
}

/**************************************************************************/
/*
    begin()

    Initializes & configures soft/bit-bang SPI
*/
/**************************************************************************/
void MAX6675Soft::begin(void)
{
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH); //disables SPI interface for MAX31855, but it will initiate measurement/conversion

  pinMode(_so, INPUT);
  pinMode(_sck, OUTPUT);
  digitalWrite(_sck, LOW); 
}

/**************************************************************************/
/*
    readRawData()

    Reads raw data from MAX6675 via software/bit-bang SPI

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
uint16_t MAX6675Soft::readRawData(void)
{
  uint16_t rawData = 0;

  digitalWrite(_cs, LOW);                        //stop  measurement/conversion
  delayMicroseconds(1);                          //4MHz  is 0.25usec, do we need it???
  digitalWrite(_cs, HIGH);                       //start measurement/conversion
  delay(MAX6675_CONVERSION_TIME);

  digitalWrite(_cs, LOW);                        //set CS low to enable SPI interface for MAX6675
  digitalWrite(_sck, LOW);

  for (int8_t i = 16; i > 0; i--)                //read 16-bits via software SPI, in order MSB->LSB (D15..D0 bit)
  {
    digitalWrite(_sck, HIGH);
    rawData = (rawData << 1) | digitalRead(_so);
    digitalWrite(_sck, LOW);
  }

  digitalWrite(_cs, HIGH);                       //disables SPI interface for MAX6675, but it will initiate measurement/conversion

  return rawData;
}
