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

const char* wifi_ssid     = "SSID";
const char* wifi_password = "PaSsWoRd";
const char* wifi_hostname = "ESP8266_xyz";

// Domoticz configuration
const char* domoticz_hostname    = "192.168.2.10";
const int   domoticz_port        = 6060;
const int   domoticz_counter_idx = 3;

// MQTT broker configuration
#define MQTT_PORT 1883
IPAddress mqtt_server_ip(192, 168, 2, 10);

#endif
