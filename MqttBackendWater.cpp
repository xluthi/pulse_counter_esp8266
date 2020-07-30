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

    MQTT backend for a pulse sensor counting water liter.
*/
#include "MqttBackendWater.h"

MqttBackendWater::MqttBackendWater(WiFiClient& wifiClient) : MqttBackend(wifiClient) {};

bool MqttBackendWater::sendLiter(float value) {
  // Send a value in liter.  This should be the best way to do it:
  // converting sensor data to physical value (liter) and then send to db.
  char msg[16];
  sprintf(msg, "%.2f", value);
  return sendMessage(MQTT_SENSOR_TOPIC, msg);
}
