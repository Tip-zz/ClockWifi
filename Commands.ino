/*
File:     Commands.ino
Origin:   04-Apr-2023
Author:   Tip Partridge

Description:
  Part of ClockWifi.ino.  This file contains user command routines

Revisions:
  04-Apr-2023 (TEP) Create from DeskClock Commands.ino
*/
//******************************************************
// shoGlob - Display global runtime variables
//******************************************************
void shoGlob()
  {
  Serial.println();
  shoNet();
  shoWifi();
  shoNTP();
  }

void shoNet()
  {
  Serial.print("Ip:            "); Serial.println(IPAddress(Net.Ip));
  Serial.print("Gateway:       "); Serial.println(IPAddress(Net.Gateway));
  Serial.print("Subnet:        "); Serial.println(IPAddress(Net.Subnet));
  Serial.print("Primary DNS:   "); Serial.println(IPAddress(Net.DNS1));
  Serial.print("Secondary DNS: "); Serial.println(IPAddress(Net.DNS2));
  }

void shoWifi()
  {
  Serial.print("Wifi SSID:     "); Serial.println(Wifi.Ssid);
  Serial.print("Wifi PW:       "); Serial.println(Wifi.Pw);
  }

void shoNTP()
  {
  Serial.print("NTP Server:    "); Serial.println(NTP.NTPServer);
  Serial.print("Time Zone:     "); Serial.println(NTP.TimeZone);
  }

//******************************************************
// shoDflt - Display default runtime variables
//******************************************************
void shoDflt()
  {
  Serial.println();
  Serial.print("Ip:            "); Serial.println(d_Ip);
  Serial.print("Gateway:       "); Serial.println(d_Gateway);
  Serial.print("Subnet:        "); Serial.println(d_Subnet);
  Serial.print("Primary DNS:   "); Serial.println(d_DNS1);
  Serial.print("Secondary DNS: "); Serial.println(d_DNS2);
  Serial.print("Wifi SSID:     "); Serial.println(d_Ssid);
  Serial.print("Wifi PW:       "); Serial.println(d_Pw);
  Serial.print("NTP Server:    "); Serial.println(d_NTPServer);
  Serial.print("Time Zone:     "); Serial.println(d_TimeZone);
  }

//******************************************************
// setP2D - Set Parameters to default values
//******************************************************
void setP2D()
  {
// load defaults to global parameters
  Net.Ip = d_Ip;
  Net.Subnet = d_Subnet;
  Net.Gateway = d_Gateway;
  Net.DNS1 = d_DNS1;
  Net.DNS2 = d_DNS2;
  strcpy(Wifi.Ssid, d_Ssid);
  strcpy(Wifi.Pw, d_Pw);
  strcpy(NTP.TimeZone, d_TimeZone);
  strcpy(NTP.NTPServer, d_NTPServer);
// Copy global parameters to EEPROM
  EEPROM.put( EEInitAddr, EEInitFlag);
  EEPROM.put( NetParamsAddr, Net);
  EEPROM.put( WifiParamsAddr, Wifi);
  EEPROM.put( NTPParamsAddr, NTP);
  EEPROM.commit();
// Report results
///  Serial.println("Global parameters set to default values.");
  }

//******************************************************
// getE2P - Copy parameters from EEPROM to global variables
//******************************************************
void getE2P()
  {
  EEPROM.get( NetParamsAddr, Net);    // get net params
  EEPROM.get( WifiParamsAddr, Wifi);  // get Wifi params
  EEPROM.get( NTPParamsAddr, NTP);    // get NTP params
// null terminate strings just in case "EEPROM" wasn't initialized.
  Wifi.Ssid[16] = 0;
  Wifi.Pw[16] = 0;
  NTP.TimeZone[32] = 0;
  NTP.NTPServer[32] = 0;
// Report results
///  Serial.println("Parameters copied from EEPROM to global parameters.");
  }
