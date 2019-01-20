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
// The value sent via MQTT is in liter, the SCALE_FACTOR convert each pulse in liter:
// value = SCALE_FACTOR * pulse
// 0.5 == one pulse per 1/2 liter
#define SCALE_FACTOR 0.5

// to be called within setup()
void setup_mqtt() {
  mqtt_client.setServer(mqtt_server_ip, MQTT_PORT);
  mqtt_client.setCallback(mqtt_callback);
  mqtt_reconnect();
  mqtt_send_boot_notification();
}

// to be called within loop()
void loop_mqtt() {
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }

  // main MQTT loop
  mqtt_client.loop();
}

// Generic MQTT callback function.
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message [%s] ", topic);
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  String s_topic(topic);

  if (s_topic == String("house/hardware/") + String(my_hostname) + String("/do_reboot")) {
    Serial.printf("!! Reboot command catched (%s): rebooting...", topic);
    ESP.restart();
  }

  // test MQTT message
  if (s_topic == String("ctrl_led")) {
  Serial.printf("ctrl_led message received, value: %c \n", (char)payload[0]);
    if ((char)payload[0] == '1') {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
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

    char topic[64];
    sprintf(topic, "house/hardware/%s/#", my_hostname);
    Serial.printf("Subscribing to MQTT topic: %s", topic);
    mqtt_client.subscribe(topic);
    mqtt_client.subscribe("ctrl_led");
  }
}

// Generic function to send a message to MQTT broker
boolean mqtt_send_message(const char *topic, const char *payload) {
  Serial.printf("Sending message '%s' to topic '%s'\n", payload, topic);
  return mqtt_client.publish(topic, payload);
}

boolean mqtt_send_boot_notification() {
  char topic[64];
  sprintf(topic, "house/hardware/%s/boot", my_hostname);
  return mqtt_send_message(topic, "1");
}

boolean mqtt_send_pulse(int value) {
  //sample MQTT messages
  float volume = value * SCALE_FACTOR;
  char msg[16];
  const char *topic = "/house/sensors/water/city";
  sprintf(msg, "%f", volume);
  return mqtt_send_message(topic, msg);
}

