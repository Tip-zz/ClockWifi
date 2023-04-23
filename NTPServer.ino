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
  DateTime.setTimeZone(NTP.TimeZone);
  DateTime.begin();
  if (!DateTime.isTimeValid())
    Serial.println(" Failed to get time from server.");
  else
    {
    Serial.print(" running   ");
    Serial.println(DateTime.toString());
    }
}

// *************************************************
