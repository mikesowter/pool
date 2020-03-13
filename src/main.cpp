#include "main.h"
#include "functions.h"

void setup()
{
	bootMillis = millis();
	Serial.begin(115200);
	Serial.println("\n\rPooltemps Rev 1.2 20190806");
	// join local network and internet
	joinNet();
  // setup connection to nano
	Wire.begin();
  // setup over the air updates
  init_OTA();
	// setup watch dog
	secondTick.attach(1,ISRwatchDog);
	// Set epoch and timers
	setupTime();
	// setup server responses
	setupServer();
	// setup file system and diag files
	setupSPIFFS();
	// setup FTP server
	ftpSrv.begin("mike","iron");
	// lookup reason for restart
	resetReason.toCharArray(charBuf,resetReason.length()+1);
	diagMess(charBuf);       // restart message
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
	if ( charBuf[16] != '0' )	{				// if fatal exception
		diagMess(charBuf);       				// usually 3 seconds into sleep
		ESP.deepSleep(600000000UL);			// sleep until the next scan
	}
	startMillis = millis();
	Serial.printf("boot delay: %i ms\n",startMillis-bootMillis);
	startMillis = millis();
	Serial.printf("boot delay: %i ms\n",startMillis - bootMillis);
}

void loop()
{
	// scan 1-wire temperature probes
	scan1Wire();
  // scan 2-wire line to nano
  scan2Wire();
	// update day if required
	if ( !onBattery ) dayCheck();
	// reset watchdog
	watchDog=0;
	// check for admin activity
	watchWait(20000UL);
	// read battery voltage
	batteryVolts = .00416 * analogRead(A0);
}

void ISRwatchDog () {
  noInterrupts();
  watchDog++;
  if (watchDog >= 120) {
    errMess("watchDog 120s");
    fd.close();
		fe.close();
    ESP.restart();
  }
	// rejoin local network if necessary
	if (WiFi.status() != WL_CONNECTED) joinNet();
	// check for scan failure
	if (millis() - lastScan > 630000UL) {
		diagMess("Prometheus 10m scan fail");
	  fd.close();
		fe.close();
    ESP.restart();
	}  
  interrupts();
}

void watchWait(uint32_t timer) {
  t0 = millis();
  while (millis()-t0 < timer) {  // wait for timeout
    if (t0>millis()) t0=millis(); // check for wrap around
    yield();
    //  check for web requests
    server.handleClient();
    // check for OTA
    ArduinoOTA.handle();
    // check for FTP request
		ftpSrv.handleFTP();
  }
}

