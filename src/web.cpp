#include <arduino.h>
#include <fs.h>
#include <ESP8266WebServer.h>
#include "functions.h"

extern char longStr[],fileSizeStr[],fileName[],userText[],charBuf[];
extern ESP8266WebServer server;
extern uint32_t fileSize, lastScan;
extern File fd, fe;
extern uint16_t longStrLen;
extern float celsius[];

void handleMetrics() {
  longStr[0]='\0';
  addCstring("\n# TYPE frProbe0 guage" );
  addCstring("\nfrProbe0 ");
  addCstring(f2s2(celsius[0]));
  addCstring("\n# TYPE frProbe1 guage" );
  addCstring("\nfrProbe1 ");
  addCstring(f2s2(celsius[1]));
  addCstring("\n# TYPE frProbe2 guage" );
  addCstring("\nfrProbe2 ");
  addCstring(f2s2(celsius[2]));
  addCstring("\n# TYPE frWifiSignal guage" );
  addCstring("\nfrWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  addCstring( "\n" );
  server.send ( 200, "text/plain", longStr );
  // shutdown for 5 minutes
  Serial.printf("scanned at %s\n",timeStamp());
  fd.close();
  fe.close();
  ESP.deepSleep(305e6); 
}

void handleNotFound() {
  server.uri().toCharArray(userText, 14);

  if (SPIFFS.exists(userText)) {
    listFile();
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
  }
  else if (strncmp(userText,"/apple",6)==0) {
  }
  else if (strncmp(userText,"/deldiags",9)==0) {
    fd.close();
    SPIFFS.remove("/diags.txt");
    fd = SPIFFS.open("/diags.txt", "a");
    diagMess("diags deleted");
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Diagnostics deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/delerrs",8)==0) {
    fe.close();
    SPIFFS.remove("/errmess.txt");
    fe = SPIFFS.open("/errmess.txt", "a");
    fe.println(dateStamp());
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Error Messages deleted<HR></head></html>");
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
