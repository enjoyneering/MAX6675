/***************************************************************************************************/
/* 
   Example for 12-bit MAX6675 K-Thermocouple to Digital Converter with Cold Junction Compensation

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/MAX6675

   - MAX31855 maximum power supply voltage is 3.0 - 5.5v
   - K-type thermocouples have an absolute accuracy of around ±2°C..±6°C.
   - Measurement tempereture range -200°C..+700°C ±2°C or -270°C..+1372°C ±6°C
     with 0.25°C resolution/increment.
   - Cold junction compensation range -40°C..+125° ±3°C with 0.062°C resolution/increment.
     Optimal performance of cold junction compensation happends when the thermocouple cold junction
     & the MAX31855 are at the same temperature. Avoid placing heat-generating devices or components
     near the converter because this may produce an errors.
   - It is strongly recommended to add a 10nF/0.01mF ceramic surface-mount capacitor, placed across
     the T+ and T- pins, to filter noise on the thermocouple lines.

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
   ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
   ESP32 Core            - https://github.com/espressif/arduino-esp32
   ESP8266 Core          - https://github.com/esp8266/Arduino
   STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <MAX6675Soft.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

uint16_t rawData     = 0;
float    temperature = 0;

/*
MAX6675Soft(cs, so, sck)

cs  - chip select
so  - serial data output
sck - serial clock input
*/

MAX6675Soft myMAX6675(2, 4, 7); //for ESP8266 change to D3 (fails to BOOT/FLASH if pin LOW), D4 (fails to BOOT/FLASH if pin LOW), D7


void setup()
{
  #if defined(ESP8266)
  WiFi.persistent(false);               //disable saving wifi config into SDK flash area
  WiFi.forceSleepBegin();               //disable AP & station by calling "WiFi.mode(WIFI_OFF)" & put modem to sleep
  #endif

  Serial.begin(115200);

  /* start MAX6675 */
  myMAX6675.begin();

  while (myMAX6675.getChipID() != MAX6675_ID)
  {
    Serial.println(F("MAX6675 error")); //(F()) saves string to flash & keeps dynamic memory free
    delay(5000);
  }
  Serial.println(F("MAX6675 OK"));
}

void loop()
{
  rawData = myMAX6675.readRawData();

  Serial.print(F("Chip ID:"));
  Serial.println(myMAX6675.getChipID(rawData));

  if   (myMAX6675.detectThermocouple(rawData) == true) Serial.println(F("K-Thermocouple is connected to MAX6675 terminals 'T+' & 'T-'"));
  else                                                 Serial.println(F("K-Thermocouple is broken, unplugged or 'T-' terminal is not grounded"));

  temperature = myMAX6675.getTemperature(rawData);

  Serial.print(F("Temperature: "));
  if   (temperature != MAX6675_ERROR) Serial.println(temperature, 1);
  else                                Serial.println(F("xx"));        //thermocouple broken, unplugged or 'T-' terminal is not grounded

  delay(1000);
}
