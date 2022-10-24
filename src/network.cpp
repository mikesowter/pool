#include "secrets.h"
#include <arduino.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

void sendNTPrequest(IPAddress& address);
unsigned long getNTPreply();
void diagMess(const char* mess);

IPAddress localIP,ausTimeServerIP;
IPAddress localTimeServerIP(192, 168, 1, 6);
IPAddress ip(192, 168, 1, 51);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);
uint32_t localPort = 2395, startSeconds; 

WiFiUDP udp;

const uint8_t NTP_PACKET_SIZE = 48;
const uint8_t BUFFER_SIZE = 100;
uint8_t byteBuf[BUFFER_SIZE];
const char* ntpServerName = "au.pool.ntp.org";
const uint8_t TIME_ZONE = 10;

void joinNet() {
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) delay(100);
 
  Serial.printf("\nIP: %u.%u.%u.%u signal: %i dBm\n",ip[0],ip[1],ip[2],ip[3],WiFi.RSSI());

  udp.begin(localPort);
  // Resolve servers
  WiFi.hostByName(ntpServerName, ausTimeServerIP);
}

unsigned long getTime() {

  unsigned long year2030 = 1893456000UL;
  unsigned long year2017 = 1483207200UL;

  uint32_t ms = millis();
  while (udp.parsePacket()!= NTP_PACKET_SIZE) {
    if (millis() - ms > 5000) {
      Serial.println("\nno reply from Aus NTP server");
      diagMess("no reply from Aus NTP server");
      break;
    }
    sendNTPrequest(ausTimeServerIP);
    delay(1000);
  }
  startSeconds = getNTPreply();
  if (startSeconds > year2017 && startSeconds < year2030) return startSeconds;

  while (udp.parsePacket()!= NTP_PACKET_SIZE) {
    if (millis() - ms > 5000) {
      diagMess("no reply from local NTP server");
      break;
    }
    sendNTPrequest(localTimeServerIP);
    delay(1000);
  }
  startSeconds = getNTPreply();
  yield();
  if (startSeconds > year2017 && startSeconds < year2030) {
    setTime(startSeconds);
    diagMess("local NTP time");
    return startSeconds;
  }
return 0;
}

// send an NTP request to the time server at the given address
void sendNTPrequest(IPAddress& address)
{
  memset(byteBuf, 0, BUFFER_SIZE);
  // Initialize values needed to form NTP request
  byteBuf[0] = 0b11100011;   // LI, Version, Mode
  byteBuf[1] = 0;     // Stratum, or type of clock
  byteBuf[2] = 6;     // Polling Interval
  byteBuf[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  byteBuf[12]  = 49;
  byteBuf[13]  = 0x4E;
  byteBuf[14]  = 49;
  byteBuf[15]  = 52;
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(byteBuf, NTP_PACKET_SIZE);
  udp.endPacket();
}

unsigned long getNTPreply(){
  udp.read(byteBuf, NTP_PACKET_SIZE); // read the packet into the buffer
  unsigned long highWord = word(byteBuf[40], byteBuf[41]);
  unsigned long lowWord = word(byteBuf[42], byteBuf[43]);
  // this is NTP time :
  unsigned long secsSince1900 = highWord << 16 | lowWord;   // seconds since Jan 1 1900
  // now convert NTP time into Unix time:
  unsigned long GMT = secsSince1900 - 2208988800UL;         // seconds since Jan 1 1970
  //add timezone offset and return
  return GMT + TIME_ZONE*3600;
}
