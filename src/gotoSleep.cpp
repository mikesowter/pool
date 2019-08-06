#include <arduino.h>
#include <fs.h>

unsigned long getTime();
extern bool sleeping;
extern File fd,fe;

void gotoSleep() {
{
  //  if not sleeping yet, start next scan
  if ( !sleeping ) {
    sleeping = true;
    return;
  }
  //  get next quarter hour
  uint32_t nextQtrSecs = 900 - getTime()%900;
  uint32_t microsToSleep = ( nextQtrSecs-30 ) * 1000000;
  fd.close();
  fe.close();
  ESP.deepSleep(microsToSleep);
  } 
}