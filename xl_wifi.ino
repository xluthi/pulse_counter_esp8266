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
void connectWifi(const char* ssid, const char* password, const char* hostname) {
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.hostname(hostname);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(getWifiInfo());
}


String getWifiInfo() {
  String res = "Wifi information : { ";

  res += "channel: ";
  res += WiFi.channel();

  res += ", physical mode: ";
  switch (WiFi.getPhyMode()) {
    case WIFI_PHY_MODE_11B:
      res += "11b";
      break;
    case WIFI_PHY_MODE_11G:
      res += "11g";
      break;
    case WIFI_PHY_MODE_11N:
      res += "11n";
      break;
  }

  res += ", mode: ";
  switch (WiFi.getMode()) {
    case WIFI_OFF:
      res += "off";
      break;
    case WIFI_STA:
      res += "station";
      break;
    case WIFI_AP:
      res += "access point";
      break;
    case WIFI_AP_STA:
      res += "access point & station";
      break;
    default:
      res += "strange mode";
  }

  res += ", RSSI: ";
  res += WiFi.RSSI();

  res += ", localIp: ";
  res += WiFi.localIP().toString();
  res += ", subnet_mask: ";
  res += WiFi.subnetMask().toString();
  res += ", gateway: ";
  res += WiFi.gatewayIP().toString();
  res += ", DNS: ";
  res += WiFi.dnsIP().toString();
  res += ", hostname: ";
  res += WiFi.hostname();
  res += "}";
  return res;
}
