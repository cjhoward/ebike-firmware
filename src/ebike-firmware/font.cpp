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

#include "font.hpp"
#include <stdint.h>

static const uint8_t segmentData[6][8] =
{
  {
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  },

  {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111
  },

  {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  },

  {
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111
  },

  {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  },

  {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b01110,
    0b01110
  }
};

static const uint8_t numberFont[11][6] =
{
  // 0
  {
    2, 0, 2,
    2, 1, 2
  },
  
  // 1
  {
    0, 2, 32,
    1, 2, 1
  },

  // 2
  {
    3, 3, 2,
    2, 3, 3
  },

  // 3
  {
    0, 3, 2,
    1, 3, 2
  },

  // 4
  {
    2, 1, 2,
    0, 0, 2
  },

  // 5
  {
    2, 3, 3,
    3, 3, 2
  },

  // 6
  {
    2, 3, 3,
    2, 1, 2
  },

  // 7
  {
    4, 0, 2,
    32, 32, 2
  },

  // 8
  {
    2, 3, 2,
    2, 3, 2
  },

  // 9
  {
    2, 3, 2,
    32, 32, 2
  },

  // Blank
  {
    32, 32, 32,
    32, 32, 32
  }
};

void loadFont(const LiquidCrystal& lcd)
{
  for (int i = 0; i < 6; ++i)
  {
    lcd.createChar(i, segmentData[i]);
  }
}

void printBig(const LiquidCrystal& lcd, int x, int number)
{
  lcd.setCursor(x, 0);
  lcd.write(numberFont[number][0]);
  lcd.write(numberFont[number][1]);
  lcd.write(numberFont[number][2]);

  lcd.setCursor(x, 1);
  lcd.write(numberFont[number][3]);
  lcd.write(numberFont[number][4]);
  lcd.write(numberFont[number][5]);
}

