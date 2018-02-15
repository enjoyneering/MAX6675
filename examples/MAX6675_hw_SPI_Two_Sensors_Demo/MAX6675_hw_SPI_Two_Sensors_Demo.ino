/***************************************************************************************************/
/* 
  Example for 12-bit MAX6675 K-Thermocouple to Digital Converter with Cold Junction Compensation

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/MAX6675

  - K-type thermocouples have an absolute accuracy of around ±2°C
  - Measurement tempereture range 0°C...+1024°C with 0.25°C resolution
  - Cold junction compensation range -20°C...+85°C
  - Keep K-type thermocouple cold junction & MAX6675 at the same temperature
  - Avoid placing heat-generating devices or components near the converter
    because this may produce errors

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <MAX6675.h>

uint16_t rawData_01     = 0;
uint16_t rawData_02     = 0;
float    temperature_01 = 0;
float    temperature_02 = 0;

/*
MAX6675(cs)

cs  - chip select, if CS low serial interface is enabled
*/

MAX6675 myMAX6675_01(4); //for ESP8266 use D4
MAX6675 myMAX6675_02(5); //for ESP8266 use D3


void setup()
{
  Serial.begin(115200);

  /* start MAX6675 */
  myMAX6675_01.begin();
  myMAX6675_02.begin();

  while (myMAX6675_01.getChipID(myMAX6675_01.readRawData()) || myMAX6675_02.getChipID(myMAX6675_02.readRawData()))
  {
    Serial.println("MAX6675_01 or MAX6675_02 error");
    delay(5000);
  }
  Serial.println("MAX6675_01 & MAX6675_02 OK");
}

void loop()
{
  rawData_01 = myMAX6675_01.readRawData();
  rawData_02 = myMAX6675_02.readRawData();

  Serial.println("Chip ID:" + myMAX6675_01.getChipID(rawData_01));
  Serial.println("Chip ID:" + myMAX6675_02.getChipID(rawData_02));

  temperature_01 = myMAX6675_01.getTemperature(rawData_01);
  temperature_02 = myMAX6675_02.getTemperature(rawData_02);

  Serial.print("Temperature_01: ");
  if (temperature_01 != MAX6675_ERROR) Serial.println(temperature_01, 1);
  else                                 Serial.println("xx");              //thermocouple broken, unplugged or 'T-' terminal is not grounded

  Serial.print("Temperature_02: ");
  if (temperature_02 != MAX6675_ERROR) Serial.println(temperature_02, 1);
  else                                 Serial.println("xx");              //thermocouple broken, unplugged or 'T-' terminal is not grounded

  delay(5000);
}
