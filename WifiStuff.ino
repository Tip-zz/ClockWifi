/*
  File:   WifiStuff.ino
  Origin: 27-Mar-2023
  Author: Tip Partridge
  Description:
     WiFi setup. Part of ClockWiFi
*/

// *************************************************

bool isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void WiFiSetup()   // common routine
{
// ------------------------------------
  Serial.print("- Wifi: ");
  int max_tries = 25;
  int pause = 500;
  int i = 0;
  WiFi.mode(WIFI_STA);
  WiFi.forceSleepWake();  // likely don't need this
  delay(200);

  if (!WiFi.config(Net.Ip, Net.Gateway, Net.Subnet, Net.DNS1))  // Need primaryDNS for NTP
    {
    Serial.println("STA Failed to configure");
    }
  Serial.print("Starting Wifi ");
  WiFi.begin(Wifi.Ssid, Wifi.Pw);
  do {
    delay(pause);
    Serial.print(".");
  } while (i++ < max_tries && !isConnected());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  if (isConnected())
    {
    Serial.println(" Wifi Started.");
    ip = WiFi.localIP();
    Serial.print("- WiFi: Connected at IP "); Serial.println(ip);
    } 
  else 
    {   
    Serial.println(" Error connecting to WiFi");
    }
}
