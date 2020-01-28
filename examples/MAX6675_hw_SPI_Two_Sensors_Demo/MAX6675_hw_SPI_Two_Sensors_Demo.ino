/***************************************************************************************************/
/* 
   Example for 12-bit MAX6675 K-Thermocouple to Digital Converter with Cold Junction Compensation

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/MAX6675

   - MAX31855 power supply voltage is 3.0 - 5.5v
   - K-type thermocouples have an absolute accuracy of around ±2°C..±6°C.
   - Measurement tempereture range -200°C..+700°C ±2°C or -270°C..+1372°C ±6°C
     with 0.25°C resolution/increment.
   - Cold junction compensation range -40°C..+125° ±3°C with 0.062°C resolution/increment.
     Optimal performance of cold junction compensation happends when the thermocouple cold junction
     & the MAX31855 are at the same temperature. Avoid placing heat-generating devices or components
     near the converter because this may produce an errors.
   - It is strongly recommended to add a 10nF/0.01mF ceramic surface-mount capacitor, placed across
     the T+ and T- pins, to filter noise on the thermocouple lines.

   This sensor uses SPI bus to communicate, specials pins are required to interface
   Board:                                    MOSI        MISO        SCLK         SS, don't use for CS   Level
   Uno, Mini, Pro, ATmega168, ATmega328..... 11          12          13           10                     5v
   Mega, Mega2560, ATmega1280, ATmega2560... 51          50          52           53                     5v
   Due, SAM3X8E............................. ICSP4       ICSP1       ICSP3        x                      3.3v
   Leonardo, ProMicro, ATmega32U4........... 16          14          15           x                      5v
   Blue Pill, STM32F103xxxx boards.......... PA17        PA6         PA5          PA4                    3v
   NodeMCU 1.0, WeMos D1 Mini............... GPIO13/D7   GPIO12/D6   GPIO14/D5    GPIO15/D8*             3v/5v
   ESP32.................................... GPIO23/D23  GPIO19/D19  GPIO18/D18   x                      3v

                                             *most boards has 10-12kOhm pullup-up resistor on GPIO2/D4 & GPIO0/D3
                                              for flash & boot

   Frameworks & Libraries:
   ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
   ESP32 Core            - https://github.com/espressif/arduino-esp32
   ESP8266 Core          - https://github.com/esp8266/Arduino
   STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <SPI.h>
#include <MAX6675.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif


uint16_t rawData_01     = 0;
uint16_t rawData_02     = 0;

float    temperature_01 = 0;
float    temperature_02 = 0;

/*
MAX6675(cs)

cs  - chip select
*/

MAX6675 myMAX6675_01(4); //for ESP8266 change to D4 (fails to BOOT/FLASH if pin LOW)
MAX6675 myMAX6675_02(5); //for ESP8266 change to D3 (fails to BOOT/FLASH if pin LOW)


void setup()
{
  #if defined(ESP8266)
  WiFi.persistent(false); //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin(); //disable swAP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
  #endif

  Serial.begin(115200);

  /* start MAX6675 */
  myMAX6675_01.begin();
  myMAX6675_02.begin();

  while ((myMAX6675_01.getChipID() != MAX6675_ID) || (myMAX6675_02.getChipID() != MAX6675_ID))
  {
    Serial.println(F("MAX6675_01 or MAX6675_02 error")); //(F()) saves string to flash & keeps dynamic memory free
    delay(5000);
  }
  Serial.println(F("MAX6675_01 & MAX6675_02 OK"));
}

void loop()
{
  rawData_01 = myMAX6675_01.readRawData();
  rawData_02 = myMAX6675_02.readRawData();

  Serial.print(F("Chip_01 ID:"));
  Serial.println(myMAX6675_01.getChipID(rawData_01));

  Serial.print(F("Chip_02 ID:"));
  Serial.println(myMAX6675_02.getChipID(rawData_02));

  temperature_01 = myMAX6675_01.getTemperature(rawData_01);
  temperature_02 = myMAX6675_02.getTemperature(rawData_02);

  Serial.print(F("Temperature_01: "));
  if (temperature_01 != MAX6675_ERROR) Serial.println(temperature_01, 1);
  else                                 Serial.println(F("xx"));           //thermocouple broken, unplugged or 'T-' terminal is not grounded

  Serial.print(F("Temperature_02: "));
  if (temperature_02 != MAX6675_ERROR) Serial.println(temperature_02, 1);
  else                                 Serial.println(F("xx"));           //thermocouple broken, unplugged or 'T-' terminal is not grounded

  delay(1000);
}
