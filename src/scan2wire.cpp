#include <arduino.h>
#include <wire.h>
void diagMess(const char* mess);
char* i2sh(uint8_t b);

extern uint8_t reply[];
extern float level,rain_t,rain_y,rain_m; 
extern float chlo1min,chlo1max,chlo1rms,chlo2min,chlo2max,chlo2rms;
char mess[50]="";

void scan2Wire() {
  uint32_t timer = millis();
  Wire.requestFrom(8, 270);    // request 270 bytes from slave device #8
  uint8_t b = 0;
  strcpy(mess,"");
  while ( Wire.available() ) {
    uint8_t c = Wire.read();  // receive a byte as character
    reply[b++] = c;
    strcat(mess,i2sh(c));
    strcat(mess," ");
    if ( millis() - timer > 1000 ) {
      diagMess("scan2Wire time-out");
      break;
    }
  }

  diagMess(mess);
  if (reply[0]=='L') {
    int reading = 256*reply[1]+reply[2];
    if ( reading > 500 && reading < 25000 ) {
      if ( level == 0 ) level = -float(reading)/100.0;
      else level = 0.99*level - 0.0001*float(reading);
    }
    chlo1min = (float)(reply[3]-127);
    chlo1max = (float)(reply[4]-127);
    chlo1rms = (float)reply[5];
    chlo2min = (float)(reply[6]-127);
    chlo2max = (float)(reply[7]-127);
    chlo2rms = (float)reply[8];
    rain_t = float(256*reply[9]+reply[10])*0.83 - rain_m;
  }
    sprintf(mess,"Level: %.1f C1min: %.1f C1max: %.1f C1rms: %.1f Rain: %.1f",level,chlo1min,chlo1max,chlo1rms,rain_t);
    diagMess(mess);
}

