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
#include "font.hpp"
#include "speedometer.hpp"

// Configuration
#define PIN_HALL 2
#define PIN_LCD_PWR 7
#define PIN_LCD_RS 12
#define PIN_LCD_EN 11
#define PIN_LCD_D4 3
#define PIN_LCD_D5 4
#define PIN_LCD_D6 5
#define PIN_LCD_D7 6
#define ADDRESS_TOP_SPEED 0x00
#define ADDRESS_TOTAL_DISTANCE (ADDRESS_TOP_SPEED + sizeof(float))
#define WHEEL_DIAMETER 670
#define HALL_SENSOR_MIN_INTERVAL 1000        // 1000 microseconds
#define HALL_SENSOR_MAX_INTERVAL 5 * 1000000 // 5 seconds
#define SPEEDOMETER_DEFAULT_DISPLAY_MODE Speedometer::DisplayMode::CURRENT_SPEED
#define SPEEDOMETER_DEFAULT_SPEED_UNIT Speedometer::SpeedUnit::MPH

template <typename T> size_t writeEEPROM(int address, const T& value);
template <typename T> size_t readEEPROM(int address, T* value);

LiquidCrystal lcd
(
  PIN_LCD_RS,
  PIN_LCD_EN,
  PIN_LCD_D4,
  PIN_LCD_D5,
  PIN_LCD_D6,
  PIN_LCD_D7
);

Speedometer& speedometer = Speedometer::instance();

void setup()
{
  // Restart LCD
  pinMode(PIN_LCD_PWR, OUTPUT);
  digitalWrite(PIN_LCD_PWR, LOW);
  delay(10);
  digitalWrite(PIN_LCD_PWR, HIGH);
  
  // Initialize LCD
  lcd.begin(16, 2);
  loadFont(lcd);

  // Read saved data from EEPROM
  float topSpeed = 0.0f;
  float totalDistance = 0.0f;
  readEEPROM<float>(ADDRESS_TOP_SPEED, &topSpeed);
  readEEPROM<float>(ADDRESS_TOTAL_DISTANCE, &totalDistance);

  // Initialize speedometer
  speedometer.init(PIN_HALL);
  speedometer.setWheelDiameter(WHEEL_DIAMETER);
  speedometer.setTopSpeed(topSpeed);
  speedometer.setTotalDistance(totalDistance);
  speedometer.setHallSensorMinimumInterval(HALL_SENSOR_MIN_INTERVAL);
  speedometer.setHallSensorMaximumInterval(HALL_SENSOR_MAX_INTERVAL);
  speedometer.setDisplayMode(SPEEDOMETER_DEFAULT_DISPLAY_MODE);
  speedometer.setSpeedUnit(SPEEDOMETER_DEFAULT_SPEED_UNIT);
}

void loop()
{
  // Get elapsed time in microseconds
  unsigned long ms = micros();

  // Save top speed and total distance to EEPROM every 30 seconds
  /*
  if (ms - distanceTime > 30 * 1000000)
  {
    writeEEPROM<float>(ADDRESS_TOTAL_DISTANCE, totalDistance);
    
    writeEEPROM<float>(ADDRESS_TOP_SPEED, topSpeed);
    distanceTime = ms;
  }
  */
  
  speedometer.update(ms);
  speedometer.display(lcd);
}

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
