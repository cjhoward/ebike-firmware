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

#ifndef SPEEDOMETER_HPP
#define SPEEDOMETER_HPP

#include <LiquidCrystal.h>

/**
 *  Speedometer singleton which uses interrupts from a Hall sensor to calculate speed and distance.
 */
class Speedometer
{
public:
  enum class DisplayMode
  {
    CURRENT_SPEED,
    CURRENT_DISTANCE
  };

  enum class SpeedUnit
  {
    KMH,
    MPH
  };
  
  /// Returns the speedometer instance
  static Speedometer& instance();
    
  /**
   * Initializes the speedometer.
   *
   * @param hallPin Pin number of the hall sensor output. This pin must support interrupts.
   * @return `true` if the speedometer was initialized successfully, false otherwise.
   */
  bool init(int hallPin);
  
  /// Updates the speedometer.
  void update(unsigned long ms);

  /// Displays the speedometer.
  void display(const LiquidCrystal& lcd);

  /// Sets the diameter of the wheel (in mm)
  void setWheelDiameter(float diameter);
  
  /// Sets the current RPM
  void setRPM(float rpm);

  /// Sets the current speed (in km/h)
  void setCurrentSpeed(float speed);

  /// Sets the top speed (in km/h)
  void setTopSpeed(float speed);

  /// Sets the current distance (in km)
  void setCurrentDistance(float distance);

  /// Sets the total distance (in km)
  void setTotalDistance(float distance);

  /// Sets the minimum duration (in ms) between Hall sensor interrupts.
  void setHallSensorMinimumInterval(unsigned long duration);

  /// Sets the maximum duration (in ms) between Hall sensor interrupts.
  void setHallSensorMaximumInterval(unsigned long duration);

  /// Sets the display mode
  void setDisplayMode(DisplayMode mode);

  /// Sets the speed unit
  void setSpeedUnit(SpeedUnit unit);

  /// Returns the total distance
  float getTotalDistance() const;

  /// Returns the top speed
  float getTopSpeed() const;

private:
  Speedometer();
  Speedometer(const Speedometer&) = delete;
  void operator=(const Speedometer&) = delete;

  /// Display the current speed on an LCD
  void displayCurrentSpeed(const LiquidCrystal& lcd);
  
  /// Handles interrupt requests from a Hall sensor
  static void hallIRQ();

  float wheelDiameter;
  float wheelCircumference;
  float rpm;
  float rpmToKMH;
  float currentSpeed;
  float topSpeed;
  float currentDistance;
  float totalDistance;
  unsigned long hallInterruptTime;
  unsigned long hallMinimumInterval;
  unsigned long hallMaximumInterval;
  DisplayMode displayMode;
  SpeedUnit speedUnit;
};

inline float Speedometer::getTotalDistance() const
{
  return totalDistance;
}

inline float Speedometer::getTopSpeed() const
{
  return topSpeed;
}

#endif // SPEEDOMETER_HPP

