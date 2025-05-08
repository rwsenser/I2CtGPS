// I2CtGPS based on GPS-play taken from Tracker9X_TX4_GPS
// Bob Senser, 2025-04-03

// This is version 1: 2025-04-23

// mixes I2C and GPS via serial
// GPS is on Serial1
// Log output on Serial (not ...1)

// note use of setup()/loop() and setup1()/loop1()
// this is multi-processor code

// I2C code based on "sender" from:
// Wire Peripheral Sender
// by Nicholas Zambetti [http://www.zambetti.com](http://www.zambetti.com)

#include <Arduino.h>
#include <string.h>

#include <Wire.h>

#undef VERBOSE
//  #define VERBOSE

#include "GPS.h"
#include "I2Cstruct.h"

GPS myGPS;
struct I2Cstruct data;

char buffer[128];
char buffer2[64];

void setup() {
  buffer[0]=0;
  data.numSats = -9999;           // marker for not data yet
  memcpy(buffer2,&data,sizeof(data));  // publish the -9999
  Wire.begin(I2C_ADDRESS);        // join i2c bus with given address
  Wire.onRequest(requestEvent);  // register event
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {

  // There is a compilter issue that prevents
  // use of strlen and print in here...???
  size_t len = sizeof(data); 
  Wire.write((char *) buffer2,len);
}
void setup1() { 
#ifdef VERBOSE  
  Serial.begin(9600); 
#endif
  Serial1.begin(9600);
  myGPS.begin(Serial1);  
}

// this really is a loop!
// work is done in requestEvent() function
void loop() {
  delay(100);
}

void loop1() {
  myGPS.loop(); 
  myGPS.ts = millis();
  if (myGPS.alt > 0) {
      sprintf(buffer, "%c%d:%ld,%d,%.2f,%f,%f",myGPS.type, myGPS.cnt,
                  (myGPS.ts / 1000), myGPS.numSats, myGPS.alt, myGPS.lat, myGPS.lng);
      // load data into shared struct
      // no mutex is used but this seems to be reliable
      // I suspect with this RP2040 chip that float and int variables
      // are copied totally with 1 hardware instruction.   This would
      // not be the case with an "UNO"....  
      data.numSats = myGPS.numSats;       
      data.cnt = myGPS.cnt;  
      data.alt = myGPS.alt;
      // correct to N/S/E/W coding
      if (myGPS.latR == 'S') data.lat = -myGPS.lat; else data.lat = myGPS.lat;
      if (myGPS.lngR == 'W') data.lng = -myGPS.lng; else data.lng = myGPS.lng;
      // the requestEvent() function want to access a buffer .... so copy tge struct???
      memcpy(buffer2,&data,sizeof(data));            
#ifdef VERBOSE
      Serial.println(buffer);
#endif       
  }
}
/* end of code */
