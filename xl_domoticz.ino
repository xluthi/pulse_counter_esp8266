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
 * Interface with domoticz via HTTP
 * Domoticz instance is supposed to run on the same LAN, without authentication, in pure HTTP (no HTTPS)
 */

#include <ESP8266HTTPClient.h>

HTTPClient http;

void sendPulseToDomoticz(int value) {
  String url = "/json.htm?type=command&param=udevice&idx=";
  url += String(domoticz_counter_idx);
  url += "&svalue=";
  url += String(value);

  sendDomoticz(url);
}

void sendDomoticz(String url){
#ifdef DEBUG
  Serial.print("connecting to ");
  Serial.println(domoticz_hostname);
  Serial.print("Requesting URL: ");
  Serial.println(url);
#endif DEBUG
  http.begin(domoticz_hostname, domoticz_port, url);
  int httpCode = http.GET();
    if (httpCode) {
      if (httpCode == 200) {
        String payload = http.getString();
#ifdef DEBUG
        Serial.println("Domoticz response "); 
        Serial.println(payload);
#endif DEBUG
      }
    }
  http.end();
}
