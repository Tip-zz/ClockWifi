/*
* File: Subroutunes.ino
* Origin: 15-Apr-2023
* Author: Tip Partridge
* Descrition:
*   User input routines
* Revision History:
*       14-Apr-2023 (TEP) Created from DeskClock Getstuff.ino.
*   
*/

#define null 0
#define bs 8
#define lf 10
#define cr 13
#define ctrlz 26
#define esc 27
#define del 127

//******** getStr *********************************
int getStr( char * str, int maxLen, int mode, const unsigned long timeout)
// Get ASCII string from Serial.
// Returns number of characters in string or -1 for timeout.
// str = pointer to null terminated return string of type char[size].
// maxLen = maximum desired string length, size-1 maximum.
// mode: 0 = ASCII, 1 = integer, 2 = float.
// timeout = inactivity timeout in ms. Null string returned for timeout.
// backspace function implemented for bs or del keys.
  {
  unsigned long time0;    // Start time for timeout.
  unsigned long timeNow;  // current time.
  char c;                 // current character.
  int cpnt = 0;           // index into str.
  str[0] = null;          // init str to zero length null terminated string.
  while (true)            // function is terminated by return statement.
    {
    time0 = millis();
    while (!Serial.available())
      {
      timeNow = millis();
      if (timeNow - time0 > timeout)
        {
        Serial.println();
        return -1;
        }
      }
    c = Serial.read();
    if (
      (mode == 0  && cpnt < maxLen && c >= ' ' && c <= '~') ||
      (mode == 1  && cpnt < maxLen && ((c >= '0' && c <= '9') || (c == '-' && cpnt == 0))) ||
      (mode == 2  && cpnt < maxLen && ((c >= '0' && c <= '9') || c == '.' || c == ',' || (c == '-' && cpnt == 0)))
      )
      {
      str[cpnt++] = c;
      Serial.print( c);
      }
    else if ((c == bs || c == del) && cpnt > 0)
      {
      Serial.print( bs);
      Serial.print( ' ');
      Serial.print( bs);
      cpnt -= 1;
      }
    else if (c == ctrlz || c == esc)
      {
      str[0] = null;
      return( 0);
      }
    else if (c == cr || c == lf)
      {
      Serial.println();
      if (cpnt >= 0)
        {
        str[cpnt] = null;   // null terminate string
        return( cpnt);
        }
      else return( 0);
      }
    }
  }

//**********************************************************
// Print formatted default value
//**********************************************************
void Default( char * dflt)
  {
  Serial.print(" [");
  Serial.print(dflt);
  Serial.print("]: ");
  }

void Default( IPAddress dflt)
  {
  Serial.print(" [");
  Serial.print(dflt);
  Serial.print("]: ");
  }

// End Subroutines
