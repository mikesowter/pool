#include <arduino.h>
#include "protocol.h"
#include <SoftwareSerial.h>
#include <fs.h>
#include "functions.h"

SoftwareSerial mySerial(5,4,false,128); 	// TX=D1=IO5, RX=D2=IO4 (Wemos mini)

extern bool invReply, firstPass;  \
extern volatile int watchDog;			
extern char longStr[];	
extern uint8_t inStr[];		
extern File fd;					

void setupInv() {
  ESP.wdtDisable();
  mySerial.begin(9600);
  while (invReply==false) {
    ESP.wdtFeed();
    mySerial.write(outStr1,11);
    watchWait(100);
    mySerial.write(outStr2,11);
    watchWait(100);
    readBytes(false);                 // read serial number
    watchWait(1000);
    mySerial.write(outStr3,28);       // assign address
    watchWait(1000);

    if (mySerial.available()==12) {
      readBytes(false);                    // read acknowledgement
      if (goodCheckSum(10)) {
        invReply = true;
        firstPass = true;             // necessary for first min/max processing
        ESP.wdtEnable(5000UL);
        return;
      }
    }
    while (mySerial.available()>0) {   // flush buffer
      mySerial.read();
      yield();
    }
    dayCheck();
    watchDog = 0;
  }
}

void readBytes(bool HexOut) {
  int i = 0;
  longStr[0]='\0';
  while (mySerial.available()>0) {
    uint8_t X = mySerial.read();
    addCstring(i2sh(X));
    addCstring(" ");
    inStr[i++] = X;
  }
  if ( i==0 || inStr[0]==0xFF ) return;
  if (HexOut) diagMess(longStr);
  fd.flush();
}
