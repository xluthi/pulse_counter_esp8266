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

    MQTT basic setup, configuration and core functions
*/
// MQTT library
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include "private.h"

#define MQTT_ROOT "house/hardware"
#define MQTT_SENSOR_ROOT "house/sensors/"
char mqtt_root[sizeof(MQTT_ROOT) + 1 + sizeof(my_hostname)];
bool send_mqtt_logs = true; // set to false to avoid sending log message via MQTT (/log topic)

// to be called within main setup()
void setup_mqtt() {
  sprintf(mqtt_root, "%s/%s", MQTT_ROOT, my_hostname);
  mqtt_client.setServer(mqtt_server_ip, MQTT_PORT);
  mqtt_client.setCallback(mqtt_callback);
  mqtt_reconnect();
  mqtt_send_log_message("I'm born. Hello World!");
  mqtt_send_boot_notification();
}

// to be called within main loop()
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

  int index = sizeof(MQTT_ROOT) + 1 + sizeof(my_hostname);
  String s_topic(topic + index); // pointer arithmetic


  /*********
   * Perform action depending on received topic
   *********/

  // reboot the ESP
  if (s_topic == "do_reboot") {
    /* See https://github.com/esp8266/Arduino/issues/1722#issuecomment-192624783
    * ESP.restart() does not work after the first restart after serial flashing.
    * However it works after manual reboot by power of RST switch.
    */
    Serial.printf("!! Reboot command catched (%s): rebooting...", topic);
    mqtt_send_log_message("OK I'm tired and I'll have to restart, please wait while I'm coming back. See you...");
    delay(1000);
    ESP.restart();
  }

  //enable or disable MQTT log
  else if (s_topic == "enable_mqtt_logs") {
    if ((char)payload[0] == '0') {
      if (send_mqtt_logs) mqtt_send_log_message("I'm stopping sending log message via MQTT. This is my last one. Bye.");
      send_mqtt_logs = false;
    } else {
      if (!send_mqtt_logs) {
        send_mqtt_logs = true;
        mqtt_send_log_message("Hello, log messages via MQTT are now activated. Nice to meet you :-)");
      }
    }
  }

  // launch firmware upgrade via HTTP download
  else if (s_topic == "do_upgrade") {
    Serial.printf("!! ESP firmware upgrade requested (%s); firmware: %s\n", topic, payload);
    char msg[70 + sizeof((char*)payload)];
    sprintf(msg, "Yeah, I'll become better by upgrading my fw to %s!\n", payload);
    mqtt_send_log_message(msg);
    HTTPUpdateResult ret = ESPhttpUpdate.update(String((char *)payload));
    if (ret == HTTP_UPDATE_FAILED) {
      // We reach this point only if the update did not work
      Serial.printf("!! Update error: %d - %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      sprintf(msg, "Oh No, upgrade didn't work :-(: %d - %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      mqtt_send_log_message(msg);
    }
  }

  // test MQTT message by controlling the builtin LED
  else if (s_topic == "ctrl_led") {
  Serial.printf("ctrl_led message received, value: %c \n", (char)payload[0]);
    if ((char)payload[0] == '0') {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Turning off the builtin LED");
      mqtt_send_log_message("OK, I'm turning off the builtin LED.");
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Turning on the builtin LED.");
      mqtt_send_log_message("OK, I'm turning on the builtin LED.");
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
    sprintf(topic, "%s/#", mqtt_root);
    Serial.printf("Subscribing to MQTT topic: %s", topic);
    mqtt_client.subscribe(topic);
  }
}

// Generic function to send a message to MQTT broker
boolean mqtt_send_message(const char *topic, const char *payload) {
  Serial.printf("Sending message '%s' to topic '%s'\n", payload, topic);
  return mqtt_client.publish(topic, payload);
}

boolean mqtt_send_boot_notification() {
  char topic[64];
  sprintf(topic, "%s/boot", mqtt_root);
  return mqtt_send_message(topic, "1");
}

boolean mqtt_send_log_message(const char *message) {
  if (!send_mqtt_logs) return false;
  char topic[sizeof(mqtt_root) + 3];
  sprintf(topic, "%s/%s", mqtt_root, "log");
  return mqtt_send_message(topic, message);
}

boolean mqtt_send_pulse(int value) {
  char msg[16];
  const char *topic = "/house/sensors/water/city";
  sprintf(msg, "%d", value);
  return mqtt_send_message(topic, msg);
}
