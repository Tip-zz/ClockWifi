/*
  File:   serialParse.ino
  Origin: 27-Mar-2023
  Author: Tip Partridge
  Description:
     Deal with USB serial commands
*/
void     serialParse()
  {
  Serial.println(C);  // Display command
  char cstr[32];      // string for gettuff
  int ier;            // getstuff return status
  bool upd;           // EEPROM update flag
  
  switch(C)
    {
    case 'A': //  A Display debug information
              if (isConnected()) Serial.println("Connected");
              else Serial.println("Not connected");
              break;
    case 'B': // Break Wifi
           ///   WiFi.config(Net.Ip, Net.Subnet, Net.Gateway, Net.DNS1);
              WiFi.begin( Wifi.Pw, Wifi.Ssid);
              break;
    case 'C': // Fix Wifi
              WiFi.begin(Wifi.Ssid, Wifi.Pw);
           ///   WiFi.config(Net.Ip, Net.Gateway, Net.Subnet, Net.DNS1);
              break;
    case 'E': // Set funky time
              if (DateTime.setTime(315576000, false))    // bool setTime(const time_t timeSecs, bool forceSet = false);
                Serial.println("Valid bogus time set");
              else
                Serial.println("Inalid bogus time set");
              break;
    case 'D': //  D  display default parameters
              Serial.println("Default Parameters");
              Serial.print  ("==================");
              shoDflt();                        // display default parameters
              Serial.println();
              break;
    case 'N': //  N  Set Network params
              upd = false;
              Serial.print("IP address");        // Get IP address
              Default(Net.Ip);
              ier = getStr( cstr, 16, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {Net.Ip.fromString( cstr); upd = true;}
              Serial.print("Subnet Mask");      // Get Subnet Mask
              Default(Net.Subnet);
              ier = getStr( cstr, 16, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {Net.Subnet.fromString( cstr); upd = true;}
              Serial.print("Gateway");          // Get Gateway
              Default(Net.Gateway);
              ier = getStr( cstr, 16, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {Net.Gateway.fromString( cstr); upd = true;}
              Serial.print("Primary DNS");      // Get Primary DNS
              Default(Net.DNS1);
              ier = getStr( cstr, 16, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {Net.DNS1.fromString( cstr); upd = true;}
              Serial.print("Secondary DNS");    // Get Secondary DNS
              Default(Net.DNS2);
              ier = getStr( cstr, 16, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {Net.DNS2.fromString( cstr); upd = true;}
              if (upd)
                {
                EEPROM.put( NetParamsAddr, Net);
                EEPROM.commit();
                Serial.println();
                Serial.println("Parameters Updated:");
                shoNet();
                WiFiSetup();                    // Try to connect
                }
              else 
                {
                Serial.println("No change.");
                getE2P();   // restore parameters from EEPROM
                }
              break;
    case 'P': //  P  display all Parameters
              Serial.println("Global Parameters");
              Serial.print  ("=================");
              shoGlob();                        // display runtime parameters
              Serial.println();
              break;
    case 'R': //  R  Restart
              Serial.print("Restarting...");    // This restarts ESP8266
              delay(2000);
              ESP.restart();
              delay(2000);
              break;
    case 'S': //  S  Set Time Params
              upd = false;
              Serial.print("NTP Server");       // Get NTP Server
              Default(NTP.NTPServer);
              ier = getStr( cstr, 32, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {strcpy( NTP.NTPServer, cstr); upd = true;}
              Serial.print("Time Zone");        // Get Time Zone
              Default(NTP.TimeZone);
              ier = getStr( cstr, 16, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {strcpy( NTP.TimeZone, cstr); upd = true;}
              if (upd)
                {
                EEPROM.put( NTPParamsAddr, NTP);
                EEPROM.commit();
                Serial.println();
                Serial.println("Parameters Updated:");
                shoNTP();
                setupDateTime();                // Try to start
                }
              else 
                {
                Serial.println("No change.");
                getE2P();   // restore parameters from EEPROM
                }
              break;
    case 'T': //  T   display date & Time
              Serial.println(DateTime.toString());
              break;
    case 'W': //  W  Set Wifi params
              upd = false;
              Serial.print("SSID"); // Get SSID
              Default(Wifi.Ssid);
              ier = getStr( cstr, 32, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {strcpy( Wifi.Ssid, cstr); upd = true;}
              Serial.print("Passcode"); // Get Passcode
              Default(Wifi.Pw);
              ier = getStr( cstr, 32, 0, 10000);
              if (ier > 0 && cstr[0] != 0) {strcpy( Wifi.Pw, cstr); upd = true;}
              if (upd)
                {
                EEPROM.put( WifiParamsAddr, Wifi);
                EEPROM.commit();
                Serial.println();
                Serial.println("Parameters Updated:");
                shoWifi();
                WiFiSetup();              // Try to connect
                }
              else 
                {
                Serial.println("No change.");
                getE2P();   // restore parameters from EEPROM
                }
              break;
    case 4:   // ^D   Load parameters with defaults
              Serial.print("Loading default global parameters...");
              setP2D();
              Serial.print("done.");
              shoGlob();
              break;
    case '?': // ?    Display menu
    case '/': Serial.println();
              Serial.println(verstr);
              Serial.println("===TIME===========================================================");
              Serial.println(" T   display date & Time");
              Serial.println("===PARAMETERS=====================================================");
              Serial.println(" P  display all Parameters        D  display default parameters");
              Serial.println(" R  Restart                      ^D  load all params with Defaults");
              Serial.println("===EDIT PARAMETERS================================================");
              Serial.println(" N  Set Network params    W  Set Wifi params    S  Set Time Server");
              Serial.println(" A - Display debug information");
              Serial.println(" B - Break Wifi");
              Serial.println(" C - Fix Wifi");
              Serial.println(" E - Set bogus time");
              break;
    }
  }
