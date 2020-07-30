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

#ifndef _MQTT_BACKEND_H_
#define _MQTT_BACKEND_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <ESP8266httpUpdate.h>


// Specific topic that all hardware on the network must listen to and answer.
// It's a kind of discovery topic to have a view on active hardware.
#define MQTT_INVENTORY_TOPIC "house/hardware/ping_all"
#define MQTT_ROOT "house/hardware"
#define MQTT_SENSOR_ROOT "house/sensors/"


class MqttBackend : public PubSubClient {
	public:
		MqttBackend(WiFiClient& wifiClient);
		// to be called within Arduino setup() global function
    void setup(const char *serverIP = "192.168.1.1", int port = 1883, const char *myId = "ESP-SENSOR");
		bool reconnect();
		// send message to a specific mqtt log topic
		bool sendLog(const char *message);

	private:
		IPAddress _serverIPAddress;
		WiFiClient* _wifiClient;  // reference to WifiClient used for ESPhttpUpdate
		int _serverPort;
		String _id;
		String _rootTopic;
		bool _doSendLogs = true; // set to false to avoid sending log message via MQTT (/log topic)

		void callback(char* topic, byte* payload, unsigned int length);
		bool connect();
		bool sendBootNotification();

	protected:
		virtual void onCallback(char* topic, byte* payload, unsigned int length);
		// Generic function to send a message to MQTT broker
		bool sendMessage(const char *topic, const char *payload);
};

#endif
