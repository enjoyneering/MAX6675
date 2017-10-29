/***************************************************************************************************/
/* 
  Example for 12-bit MAX6675 K-Thermocouple to Digital Converter with Cold Junction Compensation

  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/MAX6675

  This lcd uses I2C bus to communicate, specials pins are required to interface

  Connect chip to pins:    SDA        SCL
  Uno, Mini, Pro:          A4         A5
  Mega2560, Due:           20         21
  Leonardo:                2          3
  ATtiny85:                0(5)       2/A1(7)   (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                                 & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-01:          GPIO0/D5   GPIO2/D3  (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             GPIO4/D2   GPIO5/D1
  WeMos D1 Mini:           GPIO4/D2   GPIO5/D1

  - K-type thermocouples have an absolute accuracy of around ±2°C
  - Measurement tempereture range 0°C...+1024°C with 0.25°C resolution
  - Cold junction compensation range -20°C...+85°C
  - Keep k-type thermocouple cold junction & MAX6675 at the same temperature
  - Avoid placing heat-generating devices or components near the converter
    because this may produce errors
*/
/***************************************************************************************************/
#include <Wire.h>
#include <MAX6675.h>
#include <LiquidCrystal_I2C.h>  //https://github.com/enjoyneering/LiquidCrystal_I2C

#define LCD_ROWS           4    //qnt. of lcd rows
#define LCD_COLUMNS        20   //qnt. of lcd columns
#define LCD_DEGREE_SYMBOL  0xDF //degree symbol from lcd ROM
#define LCD_SPACE_SYMBOL   0x20 //space  symbol from lcd ROM

#define MAX_TEMPERATURE    45   //max temp, deg.C

uint8_t icon_temperature[8]  = {0x04, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0x1F, 0x0E};
float   temperature          = 0;

/*
MAX6675(cs, so, sck)

cs  - chip select, if CS low serial interface is enabled
so  - serial data output
sck - serial clock input
*/

MAX6675           myMAX6675(2, 4, 7); //use any digital pins
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);


void setup()
{
  Serial.begin(115200);

  /* LCD connection check */  
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS, LCD_5x8DOTS) != true) //20x4 display, LCD_5x8DOTS pixels size
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);
  }
  lcd.print("PCF8574 is OK");
  delay(1000);
  lcd.clear();

  /* start MAX6675 */
  myMAX6675.begin();

  while (myMAX6675.getChipID(myMAX6675.readRawData()))
  {
    lcd.setCursor(0, 0);
    lcd.print("MAX6675 error");
    delay(5000);
  }
  lcd.clear();
  lcd.print("MAX6675 OK");
  delay(2000);
  lcd.clear();

  /* load custom symbol to CGRAM */
  lcd.createChar(0, icon_temperature);

  /* prints static text */
  lcd.setCursor(0, 0);
    lcd.write((uint8_t)0);
}

void loop()
{
  temperature = myMAX6675.getTemperature(myMAX6675.readRawData());

  lcd.setCursor(2, 0);
    lcd.print(temperature, 1);
    lcd.write(LCD_DEGREE_SYMBOL);
    lcd.print("C");
    lcd.write(LCD_SPACE_SYMBOL);

    lcd.printHorizontalGraph('T', 3, temperature, MAX_TEMPERATURE); //name of the bar, 3-rd row, current value, max. value

  delay(5000);
}
