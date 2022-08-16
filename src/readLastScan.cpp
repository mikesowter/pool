/*
{
  "xhrStatus": "complete",
  "request": {
    "method": "GET",
    "url": "http://192.168.1.24:9090/api/v1/query_range?query=spRain_y&start=1655704290&end=1655790690&step=30&timeout=5s"
  },
  "response": {
    "status": "success",
    "data": {
      "resultType": "matrix",
      "result": [
        {
          "metric": {
            "__name__": "spRain_y",
            "instance": "192.168.1.51:80",
            "job": "swimmingPool"
          },
          "values": [
            [
              1655704290,
              "0"

              GET /api/v1/query_range?query=spRain_m&start=1655805585&end=1655805885&step=60&timeout=10s HTTP/1.1
Host: 192.168.1.24
Connection: close
 */

#include <arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>
 
char host[] = "192.168.1.24";   // RPi-2 prometheus and influx server
extern int16_t rain,rain_d,rain_y,rain_m,rain_mo,rain_do;
extern char longStr[];
void diagMess(const char* mess);
 
void getLastScan() {
  Serial.println("reading last good rain values");
  WiFiClient client;
  char query[160], buff[1024];
  uint16_t buffPtr, numPtr;
  uint32_t t = now()-36000;   // back to zulu time
  
  char Str1[] = "GET /api/v1/query_range?query=spRain_t";
  char Str2[] = "&start=";   
  char Str3[12];
  dtostrf((double)(t-300), 0, 0, Str3);
  char Str4[] = "&end=";
  char Str5[12];
  dtostrf((double)t, 0, 0, Str5);
  char Str6[] = "&step=60&timeout=10s HTTP/1.1\r\nHost: ";
  char Str7[] = "\r\nConnection: close\r\n\r\n";

// read most recent values

  strcpy(query,Str1);
  strcat(query,Str2);
  strcat(query,Str3);
  strcat(query,Str4);
  strcat(query,Str5);
  strcat(query,Str6);
  strcat(query,host);
  strcat(query,Str7);

  // diagMess(query);

  if (client.connect(host, 9090)) {
    client.write(query,strlen(query));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
//      sprintf(longStr,"\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
      //  diagMess(buff+numPtr);
        rain_d = atoi(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }

// read most recent rain_y

  query[37] = 'y';      

  if (client.connect(host, 9090)) {
    client.write(query,strlen(query));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
    Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
        rain_y = atoi(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }

  // read most recent spRain_m

  query[37] = 'm';      // spRain_m
  // diagMess(query);
  if (client.connect(host, 9090)) {
    client.write(query,strlen(query));
    buffPtr = 0;
    while (client.connected() || client.available()) {
      if (client.available()) {
        buff[buffPtr++] = client.read();
        if ( buffPtr > 1000 ) {
          diagMess("prometheus reply > 1000 bytes");
          break;
        }
      }
    }
    buff[buffPtr] = '\0';
    Serial.printf("\n%d bytes: \n%s\n",buffPtr,buff);
    for (numPtr = buffPtr-8; numPtr>buffPtr-18; numPtr-- ) {
      if (buff[numPtr] == '\"') {
      //  diagMess(buff+numPtr);
        rain_m = atoi(buff+numPtr+1);
      }
    }
    client.stop();
    delay(100);
  }
}
