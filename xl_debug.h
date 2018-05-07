/* 
 * (c) © 2018 Xavier Lüthi xavier@luthi.eu
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See LICENSE.txt for the full license text.
 *
 * https://forum.arduino.cc/index.php?topic=46900.0
 * 
 * Debugging tools
 */

#ifndef XL_DEBUG_H
#define XL_DEBUG_H

#ifdef DEBUG
#define DEBUG_PRINTLN(str)            \
   Serial.print(millis());            \
   Serial.print(": ");                \
   Serial.print(__PRETTY_FUNCTION__); \
   Serial.print(' ');                 \
   Serial.print(__FILE__);            \
   Serial.print(':');                 \
   Serial.print(__LINE__);            \
   Serial.print(' ');                 \
   Serial.println(str);
#else
#define DEBUG_PRINTLN(str)
#endif // DEBUG

#endif // XL_DEBUG_H

