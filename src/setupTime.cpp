#include <arduino.h>
#include <TimeLib.h>

#define R_D 0 // D3
#define R_M 2 // D4

char* dateStamp();
char* timeStamp();
unsigned long getTime();

extern uint8_t oldMin,oldQtr,oldHour,oldDay,oldMonth;
extern uint32_t t0, startMillis;
extern char todayName[], charBuf[];

extern float rain_t,rain_y,rain_m; 

void setupTime() {
  setTime(getTime());
  //setTime(23,59,30,30,11,2016);
  startMillis = millis();
  t0 = millis();
  oldMin = minute();
  oldQtr = oldMin/15;
  oldHour = hour();
  oldDay = day();
  oldMonth = month();
  strcpy(todayName,"/pt");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
}

void dayCheck() {
  if (oldDay == day()) return;
  rain_y = rain_t;
  rain_m += rain_t;
  rain_t = 0.0;
  digitalWrite(R_D,1);        // reset day in slave
  delay(1000);
  digitalWrite(R_D,0);
  if (oldMonth != month()) rain_m = 0.0;
  setupTime();
  return;
}
