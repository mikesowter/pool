#include "main.h"
#include "functions.h"

void setup()
{
	bootMillis = millis();
	Serial.begin(115200);
	Serial.println("\n\rPool Master Rev 1.6 20220526");
	// join local network and internet
	joinNet();
  // setup as I2C master
	Wire.begin();
  ThingSpeak.begin(client);
  Wire.setClock(100000UL);
  // setup LED
  pinMode(LEDPin, OUTPUT);
  // setup over the air updates
  init_OTA();
	// setup watch dog
	secondTick.attach(1,ISRwatchDog);
	// Set epoch and timers
	setupTime();
	// setup server responses
	setupServer();
	// setup file system and diag files
	setupFS();
	// setup FTP server
	ftpSrv.begin("mike","iron");
	// lookup reason for restart
	resetReason.toCharArray(charBuf,resetReason.length()+1);
	errMess(charBuf);       // restart message
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
  if ( charBuf[16] != '0' )	errMess(charBuf);       		
}

void loop()
{
  // read temperatures
	readTemps();
  // scan 2-wire line to nano
  scan2Wire();
	// scheduled activity
	if ( minute() != oldMin ) minProc();
	// reset watchdog
	watchDog=0;
	// check for admin activity
	watchWait(1000UL);
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
  if (!scanFail) {
    if (millis() - lastScan > 630000UL) {
      diagMess("Prometheus 10m scan fail");
      scanFail = true;  
    } 
  }
  else {
    if (millis() - lastScan < 63000UL) {
      diagMess("Prometheus scan restored");
      scanFail = false;  
    }
  }  
  interrupts();
}

void watchWait(uint32_t timer) {
  t0 = millis();
  digitalWrite(LEDPin,0);
  while ( millis()-t0 < timer ) {  // wait for timeout
    if ( t0 > millis() ) t0 = 0; // check for wrap around
    yield();
    //  check for web requests
    server.handleClient();
    // check for OTA
    ArduinoOTA.handle();
    // check for FTP request
		ftpSrv.handleFTP(FS_ID);
  }
  digitalWrite(LEDPin,1);
}

