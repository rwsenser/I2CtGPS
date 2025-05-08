// Wire Controller Reader
//
// by Nicholas Zambetti [http://www.zambetti.com](http://www.zambetti.com)

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI peripheral device
// Refer to the "Wire Peripheral Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

// -----------------------------------------------------------------

// 2025-04-09:
// mugged by RWS to work with I2CtGPS
// this I2C client tested with xiao rp2040, pi pico rp2040 and esp32c3
// remains public domain code
// 2025-04-29: GPStesterUNO
// UNO suppoint
// version "hacked" to work with UNO's lack of sprintf "%f" support
//

#include <Wire.h>
// #include "I2Cstruct.h"   // provides GPS data structure and I2C address
// for UNO int -> long
struct I2Cstruct {
  long numSats;
  unsigned long cnt;
  float alt;
  float lat;
  float lng;
};

#define I2C_ADDRESS (8)

char buffer[64];
char buffer2[64];
// for UNO
char salt[32];
char slat[32];
char slng[32]; 
struct I2Cstruct data;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  while (!Serial)
     delay(10);
  Serial.println("\nGPS Reader Running...");
}

void loop() {
  int len = sizeof(data);
  Wire.requestFrom(I2C_ADDRESS,len);   // address from include struct header
   // request len bytes from peripheral device 
  int i = 0;
  while (Wire.available()) { // peripheral may send less than requested
    char c = Wire.read(); // receive a byte as character
    // Serial.print(c,HEX); Serial.print(",");         // print the character
    buffer[i] = c;
    i++;
  }
  // Serial.print("\n");
  memcpy(&data, buffer,sizeof(data));
  // hack to support UNO sprint, which lacks "%f" suppoet
  // note: dtostrf(float_value, min_width, num_digits_after_decimal, where_to_store_string)
  dtostrf(data.alt,3,2,salt);
  dtostrf(data.lat,3,6,slat);
  dtostrf(data.lng,3,6,slng);
  // %d -> %ld
  sprintf(buffer2,"%ld, %ld, %s, %s, %s", data.numSats, data.cnt, salt, slat, slng );
  Serial.println(buffer2);

  delay(500); // wait half a second
}
