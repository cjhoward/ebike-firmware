/*
 * Copyright (C) 2017  Christopher J. Howard
 *
 * This file is part of E-Bike Firmware.
 *
 * E-Bike Firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * E-Bike Firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with E-Bike Firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <math.h>
#include "font.h"

#define PIN_HALL 2
#define PIN_LCD_PWR 7
#define PIN_LCD_RS 12
#define PIN_LCD_EN 11
#define PIN_LCD_D4 5
#define PIN_LCD_D5 4
#define PIN_LCD_D6 3
#define PIN_LCD_D7 6

#define ADDRESS_TOP_SPEED 0x00
#define ADDRESS_TOTAL_DISTANCE (ADDRESS_TOP_SPEED + sizeof(float))

LiquidCrystal lcd
(
  PIN_LCD_RS,
  PIN_LCD_EN,
  PIN_LCD_D4,
  PIN_LCD_D5,
  PIN_LCD_D6,
  PIN_LCD_D7
);

float topSpeed = 0.0f;
float totalDistance = 0.0f;
float speed = 0.0f;
double rpm = 0.0d;
int state = 0;
char speedString[5];
unsigned long hallTime = 0;
unsigned long distanceTime = 0;
int hallPulses = 0;
float wheelDiameter = 0.670; // (0.622 rim + 0.048 tire) (700c)
float wheelCircumference = wheelDiameter * M_PI;
float rpmToKPH = wheelCircumference * (60.0f / 1000.0f);

bool printTopSpeed = false;
bool printTotalDistance = true;

template <typename T> size_t writeEEPROM(int address, const T& value)
{
  const uint8_t* data = (const uint8_t*)(const void*)&value;
  
  size_t i;
  for (i = 0; i < sizeof(T); ++i)
    EEPROM.write(address++, *data++);

  return i;
}

template <typename T> size_t readEEPROM(int address, T* value)
{
  uint8_t* data = (uint8_t*)(void*)value;

  size_t i;
  for (i = 0; i < sizeof(T); ++i)
    *data++ = EEPROM.read(address++);

  return i;
}

void hallIRQ()
{
  unsigned long ms = micros();
  unsigned long interval = ms - hallTime;
  
  // Debounce
  if (interval < 1000)
  {
    return;
  }

  // Calculate RPM
  rpm = 60000000.0d / (double)interval;

  // Add circumference to total distance
  totalDistance += wheelCircumference;

  // Update hall time
  hallTime = ms;
}

void setup()
{
  // Restart LCD
  pinMode(PIN_LCD_PWR, OUTPUT);
  digitalWrite(PIN_LCD_PWR, LOW);
  delay(10);
  digitalWrite(PIN_LCD_PWR, HIGH);
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.noAutoscroll();
  lcd.clear();

  // Load custom font
  for (int i = 0; i < segmentCount; ++i)
  {
    lcd.createChar(i, segmentData[i]);
  }
  
  // Read saved data from EEPROM
  readEEPROM<float>(ADDRESS_TOP_SPEED, &topSpeed);
  readEEPROM<float>(ADDRESS_TOTAL_DISTANCE, &totalDistance);
  
  // Print initial screen to LCD
  /*
  lcd.setCursor(0, 1);
  lcd.print("SPEED:  0.0 km/h");
  if (printTopSpeed)
  {
    lcd.setCursor(0, 0);
    lcd.print("TOP:    0.0 km/h");
    dtostrf(topSpeed, 4, 1, speedString);
    lcd.setCursor(7, 0);
    lcd.print(speedString);
  }
  else if (printTotalDistance)
  {
    lcd.setCursor(0, 0);
    lcd.print("DIST:         km");
    dtostrf(totalDistance * 0.001, 4, 1, speedString);
    lcd.setCursor(9, 0);
    lcd.print(speedString);
  }
  */
  lcd.setCursor(12, 1);
  lcd.print("km/h");
  
  // Setup Hall sensor and IRQ
  pinMode(PIN_HALL, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_HALL), hallIRQ, RISING);
}

void loop()
{
  unsigned long ms = micros();
  
  // If too much time has passed between Hall interrupts
  if (ms - hallTime > 5 * 1000000)
  {
    // Reset RPM
    rpm = 0.0f;
  }

  // Save total distance to EEPROM every 30 seconds
  if (ms - distanceTime > 60 * 1000000)
  {
    writeEEPROM<float>(ADDRESS_TOTAL_DISTANCE, totalDistance);
    distanceTime = ms;
  }

  // Calculate speed
  speed = rpm * rpmToKPH;
  if (speed > topSpeed)
  {
    // Update top speed
    topSpeed = speed;

    // Print top speed
    if (printTopSpeed)
    {
      dtostrf(topSpeed, 4, 1, speedString);
      lcd.setCursor(7, 0);
      lcd.print(speedString);
    }
    
    // Write top speed to EEPROM
    writeEEPROM<float>(ADDRESS_TOP_SPEED, topSpeed);
  }

  if (printTotalDistance)
  {
    dtostrf(totalDistance * 0.001, 4, 1, speedString);
    lcd.setCursor(9, 0);
    //lcd.print(speedString);
  }
  
  // Print speed
  dtostrf((float)speed, 4, 1, speedString);
  lcd.setCursor(7, 1);
  //lcd.print(speedString);

  double integerPart;
  double fractionPart;
  fractionPart = modf(speed + 0.05d, &integerPart);

  int digit0 = (int)(integerPart / 10.0d);
  if (!digit0)
    digit0 = 10;
  int digit1 = (int)(integerPart) % 10;
  int digit2 = (int)(fractionPart * 10.0d);

  
  printBig(lcd, 0, digit0);
  printBig(lcd, 4, digit1);
  printBig(lcd, 8, digit2);

  lcd.setCursor(7, 1);
  lcd.write(5);
}
