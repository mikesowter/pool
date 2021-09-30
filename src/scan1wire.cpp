#include <functions.h>
#include <OneWire.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>

extern File fe;
extern OneWire ds;

uint8_t data[12], addr[8], probe, numProbes = 3;
uint16_t numSamp[3], knownAddr[3] = {0xFFAF,0xFF83,0xFFDB};

extern float celsius[3], sumTemp[5];
extern char charBuf[];

void readTemps() {
  while ( ds.search(addr)) {

    if ( OneWire::crc8(addr, 7) != addr[7] ) {
      fe=LittleFS.open("scanErrs.txt","a");
      fe.printf("\n%s %s addr = %X bad CRC!\n",dateStamp(),timeStamp(),addr);
      fe.close();
      return;
    }

    ds.reset();
    ds.select(addr);        
    ds.write(0x44,1);        // start conversion, with parasite power on

    watchWait(750UL);

    ds.reset();
    ds.select(addr);
    ds.write(0xBE,1);        // Read Scratchpad

    for ( uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
    //  printHex(data[i]);
    }
    Serial.println();
    for (uint8_t i=0; i < numProbes; i++) {
      if ((addr[1]==(knownAddr[i]>>8)) && (addr[2]==(knownAddr[i]&0xFF))) {
        probe = i;
        Serial.printf("probe: %i - ",probe);
      }
    }
    // Convert the data to actual temperature
    int16_t raw = (data[1] << 8) | data[0];
    //  check for MAX31850 thermocouple interface
    if ( addr[0] == 0x3B) {
      if (data[7] == 0x10) {
        // "count remain" gives offset adjustment
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    }
    //  check for unknown interface
    else if ( addr[0] != 0x28) {
      uint8_t cfg = (data[4] & 0x60);
      sprintf(charBuf,"\n\rnew probe: %X",cfg);
      diagMess(charBuf);
       // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      // no else - do nothing if 12 bit
    }
    if ( raw > 80 && raw < 800 ) {    // don't use unreasonable values
      celsius[probe] = (float)raw / 16.0;
      sumTemp[probe] += celsius[probe];
      numSamp[probe] += 1;
      Serial.printf("temp%d: %0.1f ",probe,celsius[probe]);
    }
  }
ds.reset_search();
return;
}
