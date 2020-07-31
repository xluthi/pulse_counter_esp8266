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
 */

#ifndef _PRIVATE_H_
#define _PRIVATE_H_

// WiFi configuration
const char* wifi_ssid     = "SSID";
const char* wifi_password = "PaSsWoRd";

// MQTT broker configuration
#define MQTT_SERVER_IP "192.168.1.7"
#define MQTT_SERVER_PORT 1883

// The MQTT topic where to send the sensor value
const char* mqtt_sensor_topic = "/house/sensors/water/city";

#endif
