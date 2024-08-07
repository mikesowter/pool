#include <arduino.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include "functions.h"

extern char longStr[], fileSizeStr[], fileName[], userText[], charBuf[];
extern ESP8266WebServer server;
extern uint32_t fileSize, lastScan, bootMillis;
extern File fd, fe;
extern FSInfo fs_info;
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
  addCstring("# TYPE spPoolTemp gauge" );
  addCstring("\nspPoolTemp ");
  addCstring(f2s2(celsius[0]));
  addCstring("\n# TYPE spPumpTemp gauge" );
  addCstring("\nspPumpTemp ");
  addCstring(f2s2(celsius[1]));
  addCstring("\n# TYPE spAirTemp gauge" );
  addCstring("\nspAirTemp ");
  addCstring(f2s2(celsius[2]));
  addCstring("\n# TYPE spSurfTemp gauge" );
  addCstring("\nspSurfTemp ");
  addCstring(f2s2(surfaceTemp));

  addCstring("\n# TYPE spPoolLevel gauge" );
  addCstring("\nspPoolLevel ");
  addCstring(f2s2(-level));
  addCstring("\n# TYPE spRawLevel gauge" );
  addCstring("\nspRawLevel ");
  addCstring(f2s2(-rawlevel));  
  addCstring("\n# TYPE spRain_t gauge" );
  addCstring("\nspRain_t ");
  addCstring(itoc(rain_d));
  addCstring("\n# TYPE spRain_y gauge" );
  addCstring("\nspRain_y ");
  addCstring(itoc(rain_y));
  addCstring("\n# TYPE spRain_m gauge" );
  addCstring("\nspRain_m ");
  addCstring(itoc(rain_m));
  
  addCstring("\n# TYPE spClmin1 gauge" );
  addCstring("\nspClmin1 ");
  addCstring(f2s2(chlo1min));
  addCstring("\n# TYPE spClmax1 gauge" );
  addCstring("\nspClmax1 ");
  addCstring(f2s2(chlo1max));
  addCstring("\n# TYPE spClrms1 gauge" );
  addCstring("\nspClrms1 ");
  addCstring(f2s2(chlo1rms));
  addCstring("\n# TYPE spClmin2 gauge" );
  addCstring("\nspClmin2 ");
  addCstring(f2s2(chlo2min));
  addCstring("\n# TYPE spClmax2 gauge" );
  addCstring("\nspClmax2 ");
  addCstring(f2s2(chlo2max));
  addCstring("\n# TYPE spClrms2 gauge" );
  addCstring("\nspClrms2 ");
  addCstring(f2s2(chlo2rms));

  if ( onBattery ) {
    addCstring("\n# TYPE spBattery gauge" );
    addCstring("\nspBattery ");
    addCstring(f2s2(batteryVolts));
    addCstring("\n# TYPE spWaitMillis gauge" );
    addCstring("\nspWaitMillis ");
    addCstring(f2s2((float)(millis()-bootMillis)));
  }
  addCstring("\n# TYPE spWifiSignal gauge" );
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
    addCstring(itoc(reply[i]));
    addCstring(",");
    addCstring(itoc(reply[i+120]));
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
    else if (strncmp(userText,"/format",7)==0) {
    fd.close();
    fe.close();
    if (!LittleFS.format()) Serial.println("LittleFS.format failed");
    LittleFS.info(fs_info);
    Serial.print(fs_info.totalBytes);
    Serial.println(" bytes available");
    Serial.print(fs_info.usedBytes);
    Serial.println(" bytes used:");
    fd = LittleFS.open("/diags.txt","a+");
    fe = LittleFS.open("/errmess.txt","a+");
    sprintf(charBuf,"reformatted,%d bytes available",fs_info.totalBytes);
    diagMess(charBuf);  
    fd.flush();
  } else {
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
