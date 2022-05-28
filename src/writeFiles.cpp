#include <arduino.h>
#include <LittleFS.h>

void diagMess(const char* mess);
char* i2sd(uint8_t b);
char* f2s2(float f);
uint8_t openFile(const char* s);
char* timeStamp();

extern File fh;
extern char charBuf[],fileName[],todayName[];
extern float celsius[], batteryVolts;
extern int16_t rain,rain_d,rain_y,rain_m,rain_mo,rain_do;
extern float level,surfaceTemp;

uint8_t storeData() {
  strcpy(fileName,todayName);
  if ( !openFile("a") ) return 0;
  fh.printf("%s,,%.1f,%.1f,%.1f,%.1f,%d,%.f\n",
    timeStamp(),celsius[0],celsius[1],celsius[2],surfaceTemp,rain,level);
  fh.close();
  return 1;
}

//----------------- open file for appending

uint8_t openFile(const char* s) {
  fh = LittleFS.open(fileName, s);
  if (!fh) {
    strcpy(charBuf,fileName);
    strcat(charBuf," failed to open");
    diagMess(charBuf);
    return 0;
  }
  return 1;
}
