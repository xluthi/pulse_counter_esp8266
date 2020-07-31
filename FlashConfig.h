/*
   Copyright (C) 2020 Xavier Lüthi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Save and retrieve some configuration parameters in flash memory.
*/
#ifndef _FLASH_CONFIG_H_
#define _FLASH_CONFIG_H_

#include <Arduino.h>
#include <EEPROM.h>

// Reserved size in bytes
#define FLASH_CONFIG_SIZE 128
// "magic" value to check if config exists already in EEPROM or not
// TODO: replace this magic value by a checksum, if possible.
#define FLASH_CONFIG_MAGIC 12345678
class FlashConfig {
  public:
    FlashConfig();
    void setup();
    void save();
    String dumpConfig();

    String getHostname();
    void setHostname(String h);

  private:
    struct sConfig {
      char hostname[32];
      uint version;
      unsigned long magic;
    } config;
    uint address;
    void loadFromEeprom();
};

#endif
