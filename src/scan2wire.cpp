#include <arduino.h>
#include <wire.h>

extern uint8_t reply[];

void scan2Wire() {
  Wire.requestFrom(8, 12);    // request 6 bytes from slave device #8
  uint8_t b = 0;
  while(Wire.available())    // slave may send less than requested
  {
    uint8_t c = Wire.read();    // receive a byte as character
    Serial.print(c,HEX);         // print the character
    reply[b++] = c;
  }
  if (reply[0]=='L') {


  }
}

