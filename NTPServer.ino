/*
  File:   NTPServer.ino
  Origin: 27-Mar-2023
  Author: Tip Partridge
  Description:
     Time server setup
*/

// *************************************************

void setupDateTime() 
{
  Serial.print("- Starting NTP client...");
  DateTime.setServer( NTP.NTPServer);
///  DateTime.setServer( NTP.NTPServer, NTP.NTPServer, NTP.NTPServer);
///  DateTime.setServer( "time-c-g.nist.got", "time-c-g.nist.gov", "time-c-g.nist.gov");
  DateTime.setTimeZone(NTP.TimeZone);
  DateTime.begin();
  if (!DateTime.isTimeValid())
    Serial.println(" Failed to get time from server.");
  else
    {
    Serial.print(" running   ");
    Serial.println(DateTime.toString());
    clockStarted = true;  // set this true once time has been received from NTP
    }
}

// *************************************************
