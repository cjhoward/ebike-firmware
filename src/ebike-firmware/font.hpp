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

#ifndef FONT_HPP
#define FONT_HPP

#include <LiquidCrystal.h>

void loadFont(const LiquidCrystal& lcd);
void printBig(const LiquidCrystal& lcd, int x, int number);

#endif // FONT_HPP

