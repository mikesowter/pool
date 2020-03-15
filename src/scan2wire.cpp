#include <arduino.h>
#include <wire.h>

extern uint8_t chlo1min,chlo1max,chlo1rms,chlo2min,chlo2max,chlo2rms,reply[];
extern float level,rain_t,rain_y,rain_m; 

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
    chlo1min = reply[3];
    chlo1max = reply[4];
    chlo1rms = reply[5];
    chlo2min = reply[6];
    chlo2max = reply[7];
    chlo2rms = reply[8];
    rain_t = float(256*reply[9]+reply[10]-10000)*0.83 - rain_m;
  }
    Serial.printf("Level: %.1f Chlor1: %d Chlor2: %d Rain: %.1f",level,chlo1rms,chlo2rms,rain_t);
}

