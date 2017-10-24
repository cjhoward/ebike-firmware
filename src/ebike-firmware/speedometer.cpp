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

#include "speedometer.hpp"
#include "font.hpp"
#include <Arduino.h>
#include <math.h>

// Converts KM/H to MPH
#define KMH_TO_MPH(x) (x * 0.6213711922)

Speedometer::Speedometer():
  wheelDiameter(0.0f),
  wheelCircumference(0.0f),
  rpm(0.0f),
  rpmToKMH(0.0f),
  currentSpeed(0.0f),
  topSpeed(0.0f),
  currentDistance(0.0f),
  totalDistance(0.0f),
  hallInterruptTime(0),
  hallMinimumInterval(0),
  hallMaximumInterval(0),
  displayMode(DisplayMode::CURRENT_SPEED),
  speedUnit(SpeedUnit::KMH)
{}

Speedometer& Speedometer::instance()
{
  static Speedometer speedometer;
  return speedometer;
}

bool Speedometer::init(int hallPin)
{
  // Setup Hall sensor IRQ
  pinMode(hallPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(hallPin), Speedometer::hallIRQ, RISING);

  return true;
}

void Speedometer::update(unsigned long ms)
{
  // Reset RPM if Hall sensor timed out
  unsigned long hallInterval = ms - hallInterruptTime;
  if (hallInterval > hallMaximumInterval)
  {
    rpm = 0.0d;
  }
  
  // Calculate current speed
  currentSpeed = rpm * rpmToKMH;

  // Update top speed
  if (currentSpeed > topSpeed)
  {
    topSpeed = currentSpeed;
  }
}

void Speedometer::display(const LiquidCrystal& lcd)
{
  if (displayMode == DisplayMode::CURRENT_SPEED)
  {
    displayCurrentSpeed(lcd);
  }
}

void Speedometer::displayCurrentSpeed(const LiquidCrystal& lcd)
{
  // Determine display speed
  float displaySpeed = 0.0f;
  if (speedUnit == SpeedUnit::KMH)
  {
    displaySpeed = currentSpeed;
  }
  else if (speedUnit == SpeedUnit::MPH)
  {
    displaySpeed = KMH_TO_MPH(currentSpeed);
  }
  
  // Separate speed number into integer part and fraction part
  double integerPart;
  double fractionPart;
  const double roundingFactor = 0.05d;
  fractionPart = modf(displaySpeed + roundingFactor, &integerPart);

  // Get two digits from the integer part and two from the fraction part
  int digits[3];
  digits[0] = (int)(integerPart / 10.0d);
  digits[1] = (int)(integerPart) % 10;
  digits[2] = (int)(fractionPart * 10.0d);

  // If digit in the tens' place is zero, print blank space
  if (digits[0] == 0)
  {
    digits[0] = 10;
  }
  
  // Print digits
  printBig(lcd, 0, digits[0]);
  printBig(lcd, 4, digits[1]);
  printBig(lcd, 8, digits[2]);

  // Print speed unit
  lcd.setCursor(12, 1);
  if (speedUnit == SpeedUnit::KMH)
  {
    lcd.print("km/h");
  }
  else if (speedUnit == SpeedUnit::MPH)
  {
    lcd.print("mph");
  }
  
  // Print decimal point
  lcd.setCursor(7, 1);
  lcd.write(5);
}

void Speedometer::setSpeedUnit(SpeedUnit unit)
{
  speedUnit = unit;
}

void Speedometer::setWheelDiameter(float diameter)
{
  // Convert wheel diameter from mm to m
  wheelDiameter = diameter * 0.001f;
  
  // Calculate wheel circumference
  wheelCircumference = wheelDiameter * M_PI;

  // Calculate factor used to convert RPM to KPH
  rpmToKMH = wheelCircumference * (60.0f / 1000.0f);
}

void Speedometer::setRPM(float rpm)
{
  this->rpm = rpm;
}

void Speedometer::setCurrentSpeed(float speed)
{
  currentSpeed = speed;
}

void Speedometer::setTopSpeed(float speed)
{
  topSpeed = speed;
}

void Speedometer::setCurrentDistance(float distance)
{
  currentDistance = distance;
}

void Speedometer::setTotalDistance(float distance)
{
  totalDistance = distance;
}

void Speedometer::setHallSensorMinimumInterval(unsigned long duration)
{
  hallMinimumInterval = duration;
}

void Speedometer::setHallSensorMaximumInterval(unsigned long duration)
{
  hallMaximumInterval = duration;
}

void Speedometer::setDisplayMode(DisplayMode mode)
{
  displayMode = mode;
}

void Speedometer::hallIRQ()
{
  Speedometer& speedometer = Speedometer::instance();
  
  // Determine duration of the interval between this interrupt and the previous interrupt
  unsigned long ms = micros();
  unsigned long interval = ms - speedometer.hallInterruptTime;
  
  // Debounce
  if (interval < speedometer.hallMinimumInterval)
  {
    return;
  }
  
  // Calculate RPM
  speedometer.rpm = 60000000.0d / (double)interval;
  
  // Add wheel circumference to distance
  speedometer.currentDistance += speedometer.wheelCircumference;
  speedometer.totalDistance += speedometer.wheelCircumference;
  
  // Update Hall sensor interrupt time
  speedometer.hallInterruptTime = ms;
}

