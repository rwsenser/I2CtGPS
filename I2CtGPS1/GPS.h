// GPS.h based on GPS-play
// Bob Senser, 2021-11-27
//   
// 2025-03-24:
// changed to honor GNGGA
// changed to use seeed xiao rp2040 (SCREEN1 for GPS)
// changed to add number of satellites in view
//
// 2025-04-06:
// debugged and renamed, GPS.h
// uses GPS data struct
//

// See license at the end of this file!

#undef GPS_PASSTHRU
// #define GPS_PASSTHRU

// VERBOSE taken from main routine

//
// configure log format 
//
#include <Arduino.h>   // required before wiring_private.h

class GPS {
  public:
  const static int BUFFER_SIZE = 64;
  char buffer[BUFFER_SIZE + 4];
  char cntBuffer[32];
  // reading variables
  int fieldCnt;
  int len;
  boolean skipLine;
  char type;
  // "payload", protected via mutex if multiprocessing is used
  volatile int cnt;
  volatile unsigned long ts;
  volatile float alt;
  volatile float lat;
  volatile char latR;
  volatile float lng;
  volatile char lngR;
  volatile int numSats;
  // end "payload"
  long int loopCnt;
  const static int TOKENSIZE = 16;
  char token[TOKENSIZE];
  Stream *gpsStream;

  GPS() { } // no ambition at all!
  
  void begin(HardwareSerial &_s) {  
    gpsStream =  &_s;
    fieldCnt = 0; 
    skipLine = false;
    strcpy(token,"");     
    type = 'G';
    cnt = 0;
    loopCnt = 0;
  }

  // for lat/log:  DD = d + (min/60) + (sec/3600)
  // here we have only degs and mins (no secs, done as fractional minutes)
  float toDegs( float _v) {
    int d = ((int) _v) / 100;   // degrees
    float m = (_v - (d * 100)) / 60.0; // minutes expressed in degrees
#if 0  
    Serial.print("DEBUG:");
    Serial.print(" ");
    Serial.print(_v);
    Serial.print(" ");
    Serial.print(d); 
    Serial.print(" "); 
    Serial.println(m);        
    // delay(1000);
#endif  
    return (d + m); 
  }

  boolean processField(int fCnt, char * t, boolean skipLine) {
    boolean skip = skipLine;
    if (!skip && (strlen(t) > 0)) {
      // Serial.print("Field: "); Serial.print(fCnt); Serial.print(" :: "); Serial.println(t);
      if (fCnt == 0 && (strcmp(t,"$GNGGA") != 0)) {  // was GPGGA
        skip = true;
      } else {
        // process the field by its number
        switch (fCnt) {
          case (2): // lat
            lat = atof(t);
            lat = toDegs(lat);
            // if (serialLevel == 2) { Serial.print(" lat: "); Serial.println(lat); }
            break;
          case (3): // lat region
            latR = t[0];
            // if (serialLevel == 2) { Serial.print(" lat r: "); Serial.println(t); }
            break;        
          case (4): // longitude
            lng = atof(t);
            lng = toDegs(lng);
            // if (serialLevel == 2) { Serial.print(" lng: "); Serial.println(lng); }
            break;       
          case (5): // longitude region
            lngR = t[0];
            // if (serialLevel == 2) { Serial.print(" lng r: "); Serial.println(t); }
            break; 
          case (7): // satellites in view
            numSats = atoi(t);
            break;         
          case (9): // altitude
            alt = atof(t) * 3.28084; // in feet
            // if (serialLevel == 2) { Serial.print(" alt: "); Serial.println(alt); }
            break;        
        }
      }
    }
    return skip;
  }

  void loop() {
#if 0
#ifdef VERBOSE
    loopCnt++;
    if (loopCnt > 1000) {
      Serial.print("!");
      loopCnt = 0;
    }
#endif   
#endif 
    if (gpsStream -> available()>0) {
      loopCnt = 0;
      len = gpsStream -> readBytes(buffer, sizeof(buffer)-1);
      buffer[len] = 0;  
#ifdef GPS_PASSTHRU
      // Serial.print(">>> ");
      // Serial.println(buffer);
      Serial.print(buffer);

#else    
      for (int k=0; k < strlen(buffer); k++) {
        char c = buffer[k];
        // Serial.print("c: "); Serial.print(c); Serial.print(" :: "); Serial.println(((int) c));
        if (c == ',') {
          skipLine = processField(fieldCnt, token, skipLine);
          fieldCnt++;  
          strcpy(token,"");          
        } else if (c == '\n' || c == '\r') {
          skipLine = processField(fieldCnt, token, skipLine);
          fieldCnt = 0;
          strcpy(token,""); 
          skipLine = false;           
        } else {  // add char to token
          char c2[2];
          c2[0] = c;
          c2[1] = 0;
          if (strlen(token) < TOKENSIZE-1) { // prevent overflow
            strcat(token,c2);
          }   
        }
      }
#endif    
    }
    cnt++;
    return;
  }
};

/* License: MIT License

Copyright (c) 2025 rwsenser

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* end of code */
