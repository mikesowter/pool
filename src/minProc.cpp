#include "functions.h"
#include <TimeLib.h>
#include <LittleFS.h>
#include <ThingSpeak.h>

extern WiFiClient client;
extern File fh,fd,fe;
extern volatile int watchDog;

const int32_t THINGSPEAK_CHANNEL = 210688;
const char * THINGSPEAK_API_KEY = "V4NN5RH9XL31ZMRM";

extern bool onBattery;          // switch to enable sleep if required

extern char fileName[];
extern char fileSizeStr[];
extern char todayName[];
extern char userText[];
extern uint16_t longStrLen;
extern char longStr[];
extern char charBuf[];
extern char dateStr[];
extern char timeStr[];

extern uint8_t oldMin, oldQtr, oldHour, oldDay, oldMonth;
extern float batteryVolts;
extern float celsius[],chlo2max,level;
extern int16_t rain,rain_d,rain_y,rain_m,rain_mo,rain_do;
extern uint32_t fileSize, secsSinceRestart;
extern uint32_t t0, t1, bootMillis, startMillis, lastScan;

void speak();

void minProc() {
  oldMin = minute();
  // flush fault files
  fd.flush();
  fe.flush();
  // check for new quarter hour
  if ( oldQtr == minute()/15 ) return;
  speak();
  storeData();                    // write day file every 15mins
  oldHour = hour();
  oldQtr = minute()/15;
  // check for end of day
  if ( day() == oldDay ) return;
  rain_y = rain_d;
  rain_d = 0;
  rain_do = rain;
  // check for end of month
  if ( month() != oldMonth ) {
    rain_m = 0;
    rain_mo = rain;
  }
  setupTime();
  return;
}  

void speak() {
  // pump temperature
  ThingSpeak.setField(1,celsius[1]);
  // pool level
  ThingSpeak.setField(2,level);
  // daily rain
  ThingSpeak.setField(3,rain_d);
  // chlorine max
  ThingSpeak.setField(4,chlo2max);

  ThingSpeak.writeFields(THINGSPEAK_CHANNEL,THINGSPEAK_API_KEY);
  watchDog = 0;
  yield();
}
