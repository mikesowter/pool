#include <fs.h>
#include <ESP8266WebServer.h>
#include "functions.h"

extern char longStr[],fileSizeStr[],fileName[],userText[],charBuf[];
extern FSInfo fs_info;
extern ESP8266WebServer server;
extern uint32_t fileSize;
extern File fh;
extern uint16_t longStrLen;

void handleDir() {
  longStr[0]='\0';
  ltoa(fs_info.usedBytes,fileSizeStr,10);
  addCstring(ltoa(fs_info.usedBytes,fileSizeStr,10));
	addCstring(" bytes used:\n");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    dir.fileName().toCharArray(fileName, 14);
    addCstring("\n");
    addCstring(fileName);
    addCstring("\t");
    itoa(dir.fileSize(),fileSizeStr,10);
    addCstring(fileSizeStr);
  }
  server.send ( 200, "text/plain", longStr );
  //Serial.println(longStr);
}

void listFile() {
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    dir.fileName().toCharArray(fileName, 14);
    if (strncmp(fileName,userText,9)==0) {
      fileSize=dir.fileSize();
      ltoa(fileSize,fileSizeStr,10);
      break;
    }
  }
  strcpy(longStr,"file: ");
  addCstring(userText);
  addCstring(" size: ");
  addCstring(fileSizeStr);
  addCstring("\r\r");
  fh = SPIFFS.open(userText, "r");

  if (fileSize > longStrLen) {
    fh.seek((longStrLen-100),SeekEnd);
  }
  while (fh.available()) {
    int k=fh.readBytesUntil('\r',charBuf,80);
    charBuf[k]='\0';
    addCstring(charBuf);
    yield();
  }
  fh.close();
  server.send ( 200, "text/plain", longStr );
}

void helpPage() {
  longStr[0]='\0';
  addCstring("Valid options include:");
  addCstring("\n");
  addCstring("8.3 filename");
  addCstring("\n");
  addCstring("day");
  addCstring("\n");
  addCstring("diags");
  addCstring("\n");
  addCstring("dir");
  addCstring("\n");
  addCstring("metrics");
  addCstring("\n");
  addCstring("deldiags");
  addCstring("\n");
  addCstring("remerrs");
  addCstring("\n");
  addCstring("reset");
  addCstring("\n");
  addCstring("shutdown");
  addCstring("\n");
  addCstring("week");
  addCstring("\n");
  server.send ( 200, "text/plain", longStr );
  //Serial.println(longStr);
}
