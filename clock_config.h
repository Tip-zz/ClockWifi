/*
  File:   clock_config.ino
  Origin: 27-Mar-2023
  Author: Tip Partridge
  Description:
     Default Wifi and Netwaork parameters. Part of ClockWiFi
*/
// Ethernet
IPAddress d_Ip(192, 168, 1, 23);
IPAddress d_Subnet(255, 255, 255, 0);
IPAddress d_Gateway(192, 168, 1, 1);
IPAddress d_DNS1(192, 168, 1, 1);   // Required for NTP
IPAddress d_DNS2(192, 168, 1, 1);
// WIFI
#define d_Ssid "WifiSSD"
#define d_Pw   "WifiPassword"
// Time Zone
#define d_TimeZone "PST8PDT,M3.2.0,M11.1.0"     // America/Los_Angeles
      // Pacific Standard Time / Pacific Daylight Time
      // Standard: month 3, week, 2, day 0 (Sun)
      // DLS:      month 11, week 1, day 0 (Sun)
// Time server
#define d_NTPServer "time-c-g.nist.gov"
//
// End clock_config
