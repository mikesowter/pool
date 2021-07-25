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

extern uint8_t oldMin, oldFive, oldHour, oldDay, oldMonth;
extern float batteryVolts;
extern float celsius[],chlo2max,level,rain_m,rain_t;
extern uint32_t fileSize, secsSinceRestart;
extern uint32_t t0, t1, bootMillis, startMillis, lastScan;

void speak();

void minProc() {
  oldMin = minute();
  // flush fault files
  fd.flush();
  fe.flush();
  // check for new quarter hour
  if ( oldFive == minute()/15 ) return;
  speak();
  storeData();                    // write day file every 15mins
  oldHour = hour();
  oldFive = minute()/15;
  // check for end of day
  if ( day() == oldDay ) return;
  setupTime();
  return;
}  

void speak() {
  // pump temperature
  ThingSpeak.setField(1,celsius[1]);
  // pool level
  ThingSpeak.setField(2,level);
  // monthly rain
  ThingSpeak.setField(3,rain_m+rain_t);
  // chlorine max
  ThingSpeak.setField(4,chlo2max);

  ThingSpeak.writeFields(THINGSPEAK_CHANNEL,THINGSPEAK_API_KEY);
  watchDog = 0;
  yield();
}
