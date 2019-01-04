/*
   Copyright (C) 2019 Xavier Lüthi

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

    MQTT basic setup and configuration
*/
// MQTT library
#include <PubSubClient.h>
#include "private.h"

// to be called within setup()
void setup_mqtt() {
  mqtt_client.setServer(mqtt_server_ip, MQTT_PORT);
  mqtt_client.setCallback(mqtt_callback);
}

// to be called within loop()
void loop_mqtt() {
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }

  // main MQTT loop
  mqtt_client.loop();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message [%s] ", topic);
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  // test MQTT message
  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void mqtt_reconnect() {
  while (!mqtt_client.connected()) {
    if (!mqtt_client.connect(my_hostname)) {
      Serial.printf("!! Error while connecting to the MQTT broker, code: %d\n", mqtt_client.state());
      delay(500);
      continue;
    }
    Serial.println("Connection to MQTT broker done :-)");
    mqtt_client.subscribe("ctrl_led");
  }
}

void mqtt_send_pulse(int value) {
  //sample MQTT messages
  char msg[16];
  char topic[64];
  sprintf(msg, "%hu", value);
  sprintf(topic, "house/esp/%s/water/city", my_hostname);
  Serial.printf("Sending message '%s' to topic '%s'\n", msg, topic);
  mqtt_client.publish(topic, msg);
}

