// an attempt at a generic include file, with minimal globals

#include <arduino.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266FTPServer.h>
#include <fs.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
#include <OneWire.h>
extern "C" {
#include "user_interface.h"
}

String resetReason = "restart: " + ESP.getResetReason();

ESP8266WebServer server(80);
FtpServer ftpSrv;
time_t getNtpTime();
FSInfo fs_info;
File fh,fd,fe;
Ticker secondTick;
volatile int watchDog = 0;
OneWire  ds(12);          // on GPIO12 - pin D6 on d1-mini

bool invReply = false;												// inverter has replied
bool firstPass = true;												// reset after inverter first replies
bool dayStored = false;												// days energy stored in totalNRG.csv
bool sleeping = false;                        // test for deepsleep system restart
bool battery = false;                         // switch to enable sleep if required

char fileName[] = "/XXyymmdd.csv";
char fileSizeStr[10];
char todayName[] = "/XXyymmdd.csv";
char userText[] = "/XXyymmdd.csv";
uint16_t longStrLen = 10000;
char longStr[10000];
char charBuf[128];
char dateStr[7];
char timeStr[10];

uint8_t oldMin, oldQtr, oldHour, oldDay, oldMonth;
int16_t sampleCount=0;
float ptQtrMax, ptQtrMin, batteryVolts;
float celsius[3], sumTemp[5];
uint32_t fileSize, secsSinceRestart;
uint32_t t0, t1, bootMillis, startMillis, lastScan;
