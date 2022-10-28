#include <arduino.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include "functions.h"

extern char longStr[], fileSizeStr[], fileName[], userText[], charBuf[];
extern ESP8266WebServer server;
extern uint32_t fileSize, lastScan, bootMillis;
extern File fd, fe;
extern uint16_t longStrLen;
extern float celsius[], batteryVolts;
extern bool onBattery;
extern int16_t rain,rain_d,rain_y,rain_m,rain_mo,rain_do;
extern float rawlevel,level,surfaceTemp;
extern float chlo1min,chlo1max,chlo1rms,chlo2min,chlo2max,chlo2rms;
extern uint8_t reply[];

void gotoSleep();

void handleMetrics() {
  longStr[0] = '\0';
  addCstring("# TYPE spPoolTemp guage" );
  addCstring("\nspPoolTemp ");
  addCstring(f2s2(celsius[0]));
  addCstring("\n# TYPE spPumpTemp guage" );
  addCstring("\nspPumpTemp ");
  addCstring(f2s2(celsius[1]));
  addCstring("\n# TYPE spAirTemp guage" );
  addCstring("\nspAirTemp ");
  addCstring(f2s2(celsius[2]));
  addCstring("\n# TYPE spSurfTemp guage" );
  addCstring("\nspSurfTemp ");
  addCstring(f2s2(surfaceTemp));

  addCstring("\n# TYPE spPoolLevel guage" );
  addCstring("\nspPoolLevel ");
  addCstring(f2s2(-level));
  addCstring("\n# TYPE spRawLevel guage" );
  addCstring("\nspRawLevel ");
  addCstring(f2s2(-rawlevel));  
  addCstring("\n# TYPE spRain_t guage" );
  addCstring("\nspRain_t ");
  addCstring(i3sd(rain_d));
  addCstring("\n# TYPE spRain_y guage" );
  addCstring("\nspRain_y ");
  addCstring(i3sd(rain_y));
  addCstring("\n# TYPE spRain_m guage" );
  addCstring("\nspRain_m ");
  addCstring(i3sd(rain_m));
  
  addCstring("\n# TYPE spClmin1 guage" );
  addCstring("\nspClmin1 ");
  addCstring(f2s2(chlo1min));
  addCstring("\n# TYPE spClmax1 guage" );
  addCstring("\nspClmax1 ");
  addCstring(f2s2(chlo1max));
  addCstring("\n# TYPE spClrms1 guage" );
  addCstring("\nspClrms1 ");
  addCstring(f2s2(chlo1rms));
  addCstring("\n# TYPE spClmin2 guage" );
  addCstring("\nspClmin2 ");
  addCstring(f2s2(chlo2min));
  addCstring("\n# TYPE spClmax2 guage" );
  addCstring("\nspClmax2 ");
  addCstring(f2s2(chlo2max));
  addCstring("\n# TYPE spClrms2 guage" );
  addCstring("\nspClrms2 ");
  addCstring(f2s2(chlo2rms));

  if ( onBattery ) {
    addCstring("\n# TYPE spBattery guage" );
    addCstring("\nspBattery ");
    addCstring(f2s2(batteryVolts));
    addCstring("\n# TYPE spWaitMillis guage" );
    addCstring("\nspWaitMillis ");
    addCstring(f2s2((float)(millis()-bootMillis)));
  }
  addCstring("\n# TYPE spWifiSignal guage" );
  addCstring("\nspWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  addCstring( "\n" );
  server.send ( 200, "text/plain", longStr );
  lastScan = millis();
  if ( onBattery ) {
    storeData();
    gotoSleep();
  }
}

void handleWave() {
  longStr[0] = '\0';
  for ( int i=16;i<136;i++) {
    addCstring(i3sd(reply[i]));
    addCstring(",");
    addCstring(i3sd(reply[i+120]));
    addCstring("\n");
  }
  server.send ( 200, "text/plain", longStr );
}

void handleNotFound() {
  server.uri().toCharArray(userText, 30);

  if (LittleFS.exists(userText)) {
    listFile();
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
  }
  else if (strncmp(userText,"/apple",6)==0) {
  }
  else if (strncmp(userText,"/deldiags",9)==0) {
    fd.close();
    LittleFS.remove("diags.txt");
    fd = LittleFS.open("diags.txt", "a");
    diagMess("diags deleted");
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Diagnostics deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/delerrs",8)==0) {
    fe.close();
    LittleFS.remove("errmess.txt");
    fe = LittleFS.open("errmess.txt", "a");
    fe.println(dateStamp());
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Error Messages deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
    else if (strncmp(userText,"/reset",6)==0) {
    fd.close();
		fe.close();
    ESP.restart();
  }
    else if (strncmp(userText,"/rainDYM",7)==0) {
    char* tok;
    tok = strtok(userText,",");
    tok = strtok(NULL,",");
    rain_d = atoi(tok);
    rain_do = rain - rain_d;
    tok = strtok(NULL,",");
    rain_y = atoi(tok);
    tok = strtok(NULL,",");
    rain_m = atoi(tok);
    rain_mo = rain - rain_m;
    sprintf(charBuf,"rain_d %d, rain_y %d, rain_m %d\n",rain_d,rain_y,rain_m);
    diagMess(charBuf);  
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>rain updated<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
    else {
    Serial.print(timeStamp());
    Serial.print(userText);
    Serial.println(" is not a valid option");
  }
//  }
}

void addCstring(const char* s) {
  // find end of longStr
  uint16_t p;
  for (p=0;p<longStrLen;p++) {
    if ( p>longStrLen-32) {
      diagMess("longStrLen exceeded");
      break;
    }
    if (longStr[p]=='\0') {
      break;    // p now points to end of old string
    }
  }
  uint16_t q=0;
  for (;p<longStrLen;p++) {
    longStr[p]=s[q];
    if (s[q++]=='\0') break;
  }
}
