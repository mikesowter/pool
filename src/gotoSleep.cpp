#include <arduino.h>
#include <fs.h>

unsigned long getTime();
void diagMess(const char* mess);
extern File fd,fe;

void gotoSleep() {
{
  diagMess("to sleep...");
  uint32_t microsToSleep = 620 * 1000000;
  fd.close();
  fe.close();
  ESP.deepSleep(microsToSleep);
  } 
}