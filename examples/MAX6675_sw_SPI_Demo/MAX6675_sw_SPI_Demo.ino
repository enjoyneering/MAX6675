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
*/
/***************************************************************************************************/
#include <MAX6675.h>

uint16_t rawData     = 0;
float    temperature = 0;

/*
MAX6675(cs, so, sck)

cs  - chip select, if CS low serial interface is enabled
so  - serial data output
sck - serial clock input
*/

MAX6675 myMAX6675(2, 4, 7); //use any digital pins


void setup()
{
  Serial.begin(115200);

  /* start MAX6675 */
  myMAX6675.begin();

  while (myMAX6675.getChipID())
  {
    Serial.println("MAX6675 error");
    delay(5000);
  }
  Serial.println("MAX6675 OK");
}

void loop()
{
  rawData = myMAX6675.readRawData();

  Serial.println("Chip ID:" + myMAX6675.getChipID(rawData));

  if   (myMAX6675.detectThermocouple(rawData) == true) Serial.println("K-Thermocouple is connected to MAX6675 terminals 'T+' & 'T-'");
  else                                                 Serial.println("K-Thermocouple is broken, unplugged or 'T-' terminal is not grounded");

  temperature = myMAX6675.getTemperature(rawData);

  Serial.print("Temperature: ");
  if   (temperature != MAX6675_ERROR) Serial.println(temperature, 1);
  else                                Serial.println("xx");           //thermocouple broken, unplugged or 'T-' terminal is not grounded

  delay(5000);
}
