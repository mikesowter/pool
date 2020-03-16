#include <arduino.h>
#include <wire.h>

extern uint8_t reply[];
extern float level,rain_t,rain_y,rain_m; 
extern float chlo1min,chlo1max,chlo1rms,chlo2min,chlo2max,chlo2rms;

void scan2Wire() {
  Wire.requestFrom(8, 12);    // request 6 bytes from slave device #8
  uint8_t b = 0;
  while(Wire.available())     // slave may send less than requested
  {
    uint8_t c = Wire.read();  // receive a byte as character
    reply[b++] = c;
  }

  if (reply[0]=='L') {
    level = float(256*reply[1]+reply[2])/100.0;
    chlo1min = (float)reply[3];
    chlo1max = (float)reply[4];
    chlo1rms = (float)reply[5];
    chlo2min = (float)reply[6];
    chlo2max = (float)reply[7];
    chlo2rms = (float)reply[8];
    rain_t = float(256*reply[9]+reply[10]-10000)*0.83 - rain_m;
  }
    Serial.printf("Level: %.1f Chlor1: %.1f Chlor2: %.1f Rain: %.1f",level,chlo1rms,chlo2rms,rain_t);
}

