/*
  File:   ClockWifi.ino
  Origin: 02-Jan-2023
  Author: Tip Partridge
  Target: Nano running Wallclock software
          NodeNCU ESP8266 with Wifi running ClockWifi software.
          Board: NodeMCU 1.0(ESP-12E Module)
          Telnet client software on remote device, like puTTY on PC.
  Description:
          This is an optional companion progran for DeskClock or WallClok.
          Note: This code is a little rough, but seems to work.
          Connecting via USB serial (115200 Baud) accesses the local menu where network parameters can be set.
          Press "?" to view the command menu.
          Connecting via telnet accesses the clock menu.
          Telnet interface is provided, Single character commands received via Wifi
          and relayed to Nano via I2C interface, which returns data via I2C which is
          then sent to telnet client. NodeMCU doesn't do slave mode with a Nano. Node Master sends
          character to Nano slave. Nano performs action and builds return data and then flags Node
          that it is ready to send via a dedication GPIO line. Node first makes data request for one
          byte which is length of data to follow. Nano transmits length and then Node makes requet for
          indicated lenght of data. Nano returns requested data. If Nano has more data then it again
          raises the dedicated GOIO line to begin another transfer. The Nano I2C Wire library only has
          32 byte transimit buffer so longet data is broken into "chunks" of 32 bytes and finished with 
          a shorter "tail" containing the remaining bytes. Eventually Node will periodically contact 
          NTP time server to periodicall reset clock and reset after power fail/startup.  May formatting
          output to VT100 emulator can advance curent state of art back into the 1980s. Not now though...
  Setup:
          Wifi uses a fixed IP address. You need to enter valid: IP, gateway, subnet mask,
          and Primary DNS using the "N" command. Then enter valid: SSID and Wifi password using the "W" command.
          After entering values you must press <Ctrl>E to save values to EEPROM.
  Revisions:
      2023-02-08 (TEP) v0.1 Little rough, but this one works.
      2023-02-21 (TEP) v0.2 Rename to WallClockWifi from I2CServer3_ESP8266.
      2023-04-12 (TEP) v0.31 Add NTP tme service
                 (TEP) v0.32 Add null termination for all EEPROM strings.
      2023-04-23 (TEP) v0.33 Code cleanup.
*/

//*******************************************************
//* Declarations *****************************************
//*******************************************************
const char verstr[] = "ClockWifi V0.33";

#include <Arduino.h>

// Wifi
#include <ESP8266WiFi.h>
#include "config.h"

// Telnet
#include "ESPTelnet.h"          
ESPTelnet telnet;
volatile bool gotWifi;
volatile char Wifi_c;

// NTP
#include "ESPDateTime.h"

// Network
#include "clock_config.h"   // defaults
struct NetParams
  {
  IPAddress Ip;
  IPAddress Subnet;
  IPAddress Gateway;
  IPAddress DNS1;
  IPAddress DNS2;
  };
struct WifiParams
  {
  char Ssid[17];
  char Pw[17];
  };
struct NTPParams
  {
  char TimeZone[33];  // Posix style timezone string
  char NTPServer[33]; // Time server URL
  };
NetParams Net;
WifiParams Wifi;
NTPParams NTP;

// I2C
#include <Wire.h>

// ***** EEPROM Stuff ************************************************
#include <EEPROM.h>
#define EESize 512
// EEPROM state
const unsigned long EEInitFlag = 1802464323L;  // this 32 bit value flags that EEPROM is initialized = 0x6b 6f 6c 43  kolC 
union Ini     // EEPROM initialized flag, can be log or array of 4 characters, just for fun
  {
  unsigned long iniflg;
  char inistr[4];
  };
Ini g_ini;              // special value indicates EEPROM has been initialized

// EEPROM layout
const int EEInitAddr = 0;   // address in EEPROM of flag that says EEPROM has been initialized
const unsigned int NetParamsAddr = EEInitAddr + sizeof(g_ini);
const unsigned int WifiParamsAddr = NetParamsAddr + sizeof(NetParams);
const unsigned int NTPParamsAddr = WifiParamsAddr + sizeof(WifiParams);

// Debug
// Include these to activate Debn() and Debnln()
//#define DEB0UG  // write
//#define DEB1UG  // request
//#define DEB2UG  // size
//#define DEB3UG  // sendTime
//#define DEB4UG  // Wifi connect
//#define DEB6UG  // 
//#define DEB7UG  // 
//#define DEB8UG  // 
//#define DEB9UG  // 
//#include <Debug_Print.h>

// ***** I2C Stuff ************************************************
  uint8_t ClientID = 1;        // Client I2C address
  uint8_t ServerID = 2;        // Server I2C address
///  unsigned long I2Cclock = 400000;   // not used
///  unsigned long I2Cclock = 100000;   // not used
const int I2C_buff_size = 33;
  // 10,000 (low speed mode), 100,000 (standard mode), 400,000 (fast mode),
  // 1,000,000 (fast mode plus), 3,400,000 (high speed mode)
volatile char I2C_str[I2C_buff_size];  // Receive data from I2C interrupt
volatile byte I2C_ptr;
#define reqPin D5    // I2C handshake input

// ***** LOOP Variables ******************************************

// Local command parser
char c, C;          // USB command, C forced to upper case
char w;             // Wifi command
bool gotc;          // Flags that character has been received from Wifi
uint8_t datLen;     // Wifi data length

// **************** SSSS **** EEEEEE *** TTTTTTTT *** UU   UU *** PPPPP ***********
// *************** SS ******* EE *********  TT ****** UU   UU *** PP   PP *********
// Setup *********** SS ***** EEEEE ******* TT ****** UU   UU *** PPPPP ***********
// ******************* SS *** EE ********** TT ****** UU   UU *** PP **************
// *************** SSSS ***** EEEEEE ****** TT ******** UUU ***** PP **************

void setup()
{
  EEPROM.begin(EESize);   // define EEPROM
  unsigned long bailTimer;
// ***** Init USB serial and say Hi *****************************
  Serial.begin(115200);
  while (!Serial) { }
  delay(200);  
  Serial.println();
  Serial.println(verstr);

// ***** Optional setup bail *************************************
// This bypasses setup.  ESP may crash for a number of reasons, including
// trying to read uninitialized EEPROM.
  Serial.println("Press any key to bypass setup...");
  bailTimer = millis();
  while (millis() - bailTimer < 1000)
    {
    if (Serial.available())
      {
      while (Serial.available()) {Serial.read();}
      Serial.println("Setup bypassed.");
      return;
      }
    }

// ***** check EEPROM status *************************************
  EEPROM.get( EEInitAddr, g_ini.iniflg);
  if (g_ini.iniflg == EEInitFlag) // EEPROM initialized?
    {
    Serial.print("- Loading parameters from EEPROM...");
    getE2P();       // load global parameters from EEPROM
    Serial.println(" done.");
    }
  else              // EEPROM blank, use defaults
    {
    Serial.print("- Initializing EEPROM...");
    setP2D();       // Load default parameters
    Serial.println(" done.");
    }

// ***** Set up I2C ******************************************
////  Wire.setClock(I2Cclock);
  Wire.begin();
  pinMode( reqPin, INPUT);

// ***** Wifi Setup ******************************************
  WiFiSetup();

// ***** NPT Setup ******************************************
  if (isConnected()) setupDateTime();

// ***** NPT Setup ******************************************
  if (isConnected()) setupTelnet();   // Telnet configuration

// ***** Prompt *********************************************
  Serial.println("Press ? for menu.");
}

//******************* MM      MM ****** AAA ******* IIII *** NN    NN **************************
//******************* MMMM  MMMM ***** AA AA ******* II **** NNN   NN **************************
// Main loop ******** MM  MM  MM **** AAAAAAA ****** II **** NN NN NN **************************
//******************* MM      MM *** AA     AA ***** II **** NN   NNN **************************
//******************* MM      MM ** AA       AA *** IIII *** NN    NN **************************

void loop()
{
//
// ===== Get a character ===============================
//

  telnet.loop();  // not sure what this does, but seems essential to have it in the loop.

// Check USB
  if (Serial.available())
    {
    c = Serial.read();    // Get character from USB Serial
    C = (toupper(c));
    serialParse();
    }

// Check WIFI
  gotc = false;  // none yet
  if (gotWifi)
    {
    gotWifi = false;
    gotc = true;
    w = Wifi_c;
    }
//
// ===== Parse Wifi character ===============================
//
// Check if we have received a new command
  if (gotc)
    {
    Wire.beginTransmission(ClientID);
    Wire.write(w);
    Wire.endTransmission();        // default releases bus
    gotc = false;
    }
// Check if client has data
if (digitalRead( reqPin))
  {
  Wire.requestFrom(ClientID, (uint8_t)1); // request & read 1 byte data size from slave 
  while(Wire.available())
    {
    datLen = Wire.read(); // Number of bytes client wants to send
    }
  if (datLen == 0xff) {sendTime();}
  else
    {
    if (datLen > 32) datLen = 32;
    Wire.requestFrom(ClientID, datLen); // request & read size bute data from slave
    while(Wire.available())
      {
      char x = Wire.read();
      telnet.print(x);
      }
    }
  }
} // End of Loop

//********************************************************************************
//********************************************************************************
// End Main loop *****************************************************************
//********************************************************************************
//********************************************************************************

// ***** Send time from NTP **********************************

void sendTime()
  {
  String timStr;
  byte datTim[7];
  int i;
  timStr = DateTime.toString();
  datTim[0] = 10*(timStr[0]-'0') + (timStr[1]-'0');
  datTim[1] = 10*(timStr[2]-'0') + (timStr[3]-'0');
  datTim[2] = 10*(timStr[5]-'0') + (timStr[6]-'0');
  datTim[3] = 10*(timStr[8]-'0') + (timStr[9]-'0');
  datTim[4] = 10*(timStr[11]-'0') + (timStr[12]-'0');
  datTim[5] = 10*(timStr[14]-'0') + (timStr[15]-'0');
  datTim[6] = 10*(timStr[17]-'0') + (timStr[18]-'0');
  Wire.beginTransmission(ClientID);
  for (i=0; i<7; i++)
    {
    Wire.write(datTim[i]);
    }
  Wire.endTransmission();        // default releases bus
  }

//****************************************************
