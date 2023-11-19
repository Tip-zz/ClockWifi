/*
  File:   TelnetServer.ino
  Origin: 27-Mar-2023
  Author: Tip Partridge
  Description:
     Telnet server setup and ISRs
*/

/* ------------------------------------------------- */

IPAddress ip;
uint16_t  port = 23;

/* ------------------------------------------------- */

/* ------------------------------------------------- */

void errorMsg(String error, bool restart = true) {
  Serial.println(error);
  if (restart) {
    Serial.println("Rebooting now...");
    delay(2000);
    ESP.restart();
    delay(2000);
  }
}

void setupTelnet() {  
// passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  telnet.onInputReceived(onTelnetInputReceived);

  telnet.setLineMode(false);
  Serial.print("- Telnet: ");
  if (telnet.begin(port)) 
    {
    Serial.println("running");
    telnetStarted = true;
    } 
  else 
    {
      Serial.println("error.");
      errorMsg("Will reboot...");
    }
}

/* ------------------------------------------------- */

/* ================================================= */
// telnet receive interrupt service routine
//    get string from wifi
//    put first character into global char Telnet_c
//    set global flag bool gotTelnet if character isn't a null
void onTelnetInputReceived(String str)
  {
//  Serial.print(str);
  Telnet_c = str[0];                    // get first character
  if (Telnet_c != 0) gotTelnet = true;  // set flag if it's not a null
  }
/* ================================================= */

/* ------------------------------------------------- */

// (optional) callback functions for telnet events
void onTelnetConnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" connected");
  telnet.println(verstr);
  telnet.println("Welcome " + telnet.getIP());
  telnet.println("Press ? for menu.");
}

void onTelnetDisconnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" disconnected");
}

void onTelnetReconnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" reconnected");
}

void onTelnetConnectionAttempt(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" tried to connected");
}

/* ------------------------------------------------- */
