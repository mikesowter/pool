// battery powered, intermittent monitor for swimming pool

#include "main.h"
#include "functions.h"

void setup()
{
	bootMillis = millis();
	Serial.begin(115200);
	Serial.println("\n\rPooltemps Rev 1.1 20190721");
	// join local network and internet
	joinNet();
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
	if( charBuf[9] == 'D' ) sleeping = true;
	diagMess(charBuf);       // restart message
	startMillis = millis();
	Serial.printf("boot delay: %i ms\n",startMillis - bootMillis);
}

void loop()
{
	// scan 1-wire temperature probes
	scan1Wire();
	// periodic processing
	if ( minute() != oldMin ) minProc();
	// reset watchdog
	watchDog=0;
	// check for activity
	watchWait(20000UL);
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
	if (millis() - lastScan > 330000UL) {
		diagMess("Prometheus 5m scan fail");
	  fd.close();
		fe.close();
		if (battery) ESP.deepSleep(300e6); 
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

