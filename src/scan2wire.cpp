#include <arduino.h>
#include <wire.h>
void diagMess(const char* mess);
char* i2sh(uint8_t b);
uint32_t timer;
bool framed;
extern bool reboot;
extern uint8_t reply[];
extern float level,rain_t,rain_y,rain_m; 
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
    int reading = 256*reply[1]+reply[2];
    if ( reading > 500 && reading < 25000 ) {
      if ( level == 0 ) level = -float(reading)/100.0;
      else level = 0.99*level - 0.0001*float(reading);
    }
    rain_t = float(256*reply[3]+reply[4])*0.83;
    if ( reboot ) {     // compromise, but works well later in month
      rain_m = rain_t;
      reboot = false;
    } 
    rain_t -= rain_m;

    chlo1min = (float)(reply[5]-127);
    chlo1max = (float)(reply[6]-127);
    chlo1rms = (float)reply[7]/4.0;
    chlo2min = (float)(reply[8]-127);
    chlo2max = (float)(reply[9]-127);
    chlo2rms = (float)reply[10]/4.0; 
  }

//    sprintf(mess,"Level: %.1f C1min: %.1f C1max: %.1f C1rms: %.1f Rain: %.1f",level,chlo1min,chlo1max,chlo1rms,rain_t);
//    diagMess(mess);
}

