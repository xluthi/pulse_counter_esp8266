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

    MQTT backend with High Level functions.
*/

#include <Arduino.h>
#include <string.h>
#include "MqttBackend.h"

MqttBackend::MqttBackend(WiFiClient& wifiClient) : PubSubClient(wifiClient) {
	this->_wifiClient = &wifiClient;
	this->_config = NULL;
};

void MqttBackend::setup(const char* serverIP, int port, String myId) {
	this->_serverIPAddress.fromString(serverIP);
	this->_serverPort = port;
	setServer(_serverIPAddress, _serverPort);
	this->_id = myId;
	setRootTopic();

	reconnect();
	sendLog("I'm born. Hello World!");
	sendBootNotification();
}

void MqttBackend::setRootTopic() {
	this->_rootTopic = String(MQTT_ROOT) + "/" + _id;
}

void MqttBackend::setFlashConfig(FlashConfig& config) {
	_config = &config;
}


bool MqttBackend::reconnect() {
  // Loop until we're reconnected
  while (!this->connected()) {
    if (!PubSubClient::connect(_id.c_str())) {
      Serial.printf("!! Error while connecting to the MQTT broker, code: %d\n", this->state());
      // Wait 1 second before retrying
      delay(1000);
      continue;
    }
    Serial.println("Connection to MQTT broker done :-)");

		String topic  = _rootTopic + "/#";
    Serial.printf("Subscribing to MQTT topic: %s", topic.c_str());
    this->subscribe(topic.c_str());
    this->subscribe(MQTT_INVENTORY_TOPIC);
  }
	return true;
}

bool MqttBackend::forceReconnect() {
	disconnect();
	return reconnect();
}

bool MqttBackend::sendMessage(const char *topic, const char *payload) {
	Serial.printf("Sending message '%s' to topic '%s'\n", payload, topic);
	return this->publish(topic, payload);
}

bool MqttBackend::sendBootNotification() {
  return sendMessage(String(_rootTopic + "/boot").c_str(), "1");
}

bool MqttBackend::sendLog(const char *message) {
  if (!_doSendLogs) return false;
  return sendMessage(String(_rootTopic + "/log").c_str(), message);
}

void MqttBackend::onCallback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message [%s] ", topic);
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  String s_topic = String(topic).substring(_rootTopic.length() + 1);


  /*********
   * Perform action depending on received topic
   *********/
  // send my identity (kind of inventory)
  if (String(topic) == String(MQTT_INVENTORY_TOPIC)) {
    delay(random(1000));
    char msg[100];
    sprintf(msg, "Nice to meet you, I'm %s, sending on topic %s.", _id.c_str(), MQTT_SENSOR_ROOT);
    sendLog(msg);
    char topic[60];
    sprintf(topic, "%s/%s", MQTT_INVENTORY_TOPIC, "result");
    sendMessage(topic, _id.c_str());
  }
  // reboot the ESP
  else if (s_topic == "do_reboot") {
    /* See https://github.com/esp8266/Arduino/issues/1722#issuecomment-192624783
    * ESP.restart() does not work after the first restart after serial flashing.
    * However it works after manual reboot by power of RST switch.
    */
    Serial.printf("!! Reboot command catched (%s): rebooting...", topic);
    sendLog("OK I'm tired and I'll have to restart, please wait while I'm coming back. See you...");
    delay(1000);
    ESP.restart();
  }

  //enable or disable MQTT log
  else if (s_topic == "enable_mqtt_logs") {
    if ((char)payload[0] == '0') {
      if (_doSendLogs) sendLog("I'm stopping sending log message via MQTT. This is my last one. Bye.");
      _doSendLogs = false;
    } else {
      if (!_doSendLogs) {
        _doSendLogs = true;
        sendLog("Hello, log messages via MQTT are now activated. Nice to meet you :-)");
      }
    }
  }

  // launch firmware upgrade via HTTP download
  else if (s_topic == "do_upgrade") {
    Serial.printf("!! ESP firmware upgrade requested (%s); firmware: %s\n", topic, payload);
    char msg[70 + sizeof((char*)payload)];
    sprintf(msg, "Yeah, I'll become better by upgrading my fw to %s!\n", payload);
    sendLog(msg);
    HTTPUpdateResult ret = ESPhttpUpdate.update(*_wifiClient, String((char *)payload));
    if (ret == HTTP_UPDATE_FAILED) {
      // We reach this point only if the update did not work
      Serial.printf("!! Update error: %d - %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      sprintf(msg, "Oh No, upgrade didn't work :-(: %d - %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      sendLog(msg);
    }
  }

  // test MQTT message by controlling the builtin LED
  else if (s_topic == "ctrl_led") {
  Serial.printf("ctrl_led message received, value: %c \n", (char)payload[0]);
    if ((char)payload[0] == '0') {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Turning off the builtin LED");
      sendLog("OK, I'm turning off the builtin LED.");
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Turning on the builtin LED.");
      sendLog("OK, I'm turning on the builtin LED.");
    }
  }

  // return some configuration and state information's
  else if (s_topic == "get_config") {
    char msg[100];
    sprintf(msg, "I'm sending info to '%s' topic.", _rootTopic.c_str());
    sendLog(msg);
    sprintf(msg, "File %s compiled on %s %s.", __FILE__, __DATE__, __TIME__);
    sendLog(msg);
  }

	// update hostname of the ESP
	else if (s_topic == "set_hostname") {
		char hostname[length+1];
		strncpy(hostname, (char*) payload, length);
		hostname[length] = 0;
		Serial.print("set_hostname received with new hostname :");
		Serial.println(hostname);
		this->_id = String(hostname);
		setRootTopic();
		//disconnect and reconnect to update Hostname
		Serial.println("I will now disconnect and reconnect with my new hostname");
		forceReconnect();
		if (_config) _config->setHostname(_id);
	}

	else if (s_topic == "reset_hostname") {
		char hostname[12];
		sprintf(hostname, "ESP-%06X", ESP.getChipId());
		this->_id = String(hostname);
		setRootTopic();
		forceReconnect();
		if (_config) _config->setHostname(_id);
	}

	else if (s_topic == "dump_flash") {
		if(!_config) return;
		sendLog(_config->dumpConfig().c_str());
	}
}
