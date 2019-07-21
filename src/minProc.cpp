#include <fs.h>

uint8_t storeData();

extern uint8_t oldMin,oldQtr,oldHour,oldDay,oldMonth;

void minProc() {
  oldMin = minute();
  if (oldMin/15 == oldQtr) return;
  storeData();
  oldQtr = oldMin/15;
  oldHour = hour();
  return;
}
