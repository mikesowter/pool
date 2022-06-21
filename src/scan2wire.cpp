#include <arduino.h>
#include <wire.h>
void diagMess(const char* mess);
char* i2sh(uint8_t b);
uint32_t timer;
bool framed;
extern bool reboot;
extern uint8_t reply[];
extern float level,surfaceTemp;
extern int16_t rain,rain_d,rain_y,rain_m,rain_mo,rain_do;
extern float chlo1min,chlo1max,chlo1rms,chlo2min,chlo2max,chlo2rms;
char mess[100] = "";

void scan2Wire() {
  uint16_t b = 0;
  framed = false;
  timer = millis();
  for ( uint8_t p=0; p<8; ) {     // 8 packets of 32 -> 256 byte reply
    Wire.requestFrom(8, 32);      // request 32 bytes from slave device #8
    strcpy(mess,"");
    delay(100);
    while ( Wire.available() ) {
      uint8_t c = Wire.read();    // receive a byte as character
      if ( !framed && b==0 && c=='L' ) {  // framing for reply
        p = 0;   
        framed = true;
      }
      reply[b++] = c;
      strcat(mess,i2sh(c));
      strcat(mess," ");

      yield();
    }
    p++;
  //  diagMess(mess);
  }
  //  Serial.printf("\nMillis: %lu\n",millis()-timer);

  
  if (reply[0]=='L') {
    float newLevel = -float(256*reply[1]+reply[2])/100.0;
    if ( level == 0 ) level = newLevel;
    else if ( newLevel > level-10 && newLevel < level+50 ) {
      level = 0.99*level + 0.01*float(newLevel);
    }   
    rain = 256*reply[3]+reply[4];  // 1 count = 1mm Dec 2020

  // rain upcounts to 33m, resets on slave restart, rain added to log    
    if (rain == 0 || reboot) { // if slave or master restarted 
      rain_do = rain - rain_d;  
      rain_mo = rain - rain_m;  
      reboot = false;
    }
    rain_d = rain - rain_do;       // daily offset set in minProc
    rain_m = rain - rain_mo;       // monthly offset set in minProc

    surfaceTemp = float(256*reply[5]+reply[6])/16.0;  

    chlo1min = (float)(reply[10]-127);
    chlo1max = (float)(reply[11]-127);
    chlo1rms = (float)reply[12]/4.0;
    chlo2min = (float)(reply[13]-127);
    chlo2max = (float)(reply[14]-127);
    chlo2rms = (float)reply[15]/4.0; 
  }

//    sprintf(mess,"Level: %.1f C1min: %.1f C1max: %.1f C1rms: %.1f Rain: %.1f",level,chlo1min,chlo1max,chlo1rms,rain_d);
//    diagMess(mess);
}


