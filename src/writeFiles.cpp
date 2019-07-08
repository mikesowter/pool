#include <arduino.h>
#include <fs.h>

void diagMess(const char* mess);
char* i2sd(uint8_t b);
char* f2s2(float f);
uint8_t openFile(const char* s);
char* timeStamp();

extern File fh;
extern char charBuf[],fileName[],todayName[];
extern float celsius[];

uint8_t storeData() {
  strcpy(fileName,todayName);
  if ( !openFile("a") ) return 0;
  fh.printf("%s %.1f,%.1f,%.1f\n",timeStamp(),celsius[0],celsius[1],celsius[2]);
  fh.close();
  return 1;
}

//----------------- open file for appending

uint8_t openFile(const char* s) {
  fh = SPIFFS.open(fileName, s);
  if (!fh) {
    strcpy(charBuf,fileName);
    strcat(charBuf," failed to open");
    diagMess(charBuf);
    return 0;
  }
  return 1;
}
