#include <WiFiClient.h>

// main
void ISRwatchDog ();
// network
void joinNet();
unsigned long getTime();
void sendNTPrequest(IPAddress& address);
unsigned long getNTPreply();
// OTA
uint8_t init_OTA();
// diags
void diagMess(const char* mess);
void errMess(const char* mess);
char* dateStamp();
char* timeStamp();
void printHex(uint8_t X);
char* i2sh(uint8_t b);
char* i2sd(uint8_t b);
char* itoc(int16_t b);
char* f2s2(float f);
// main
void watchWait(uint32_t timer);
// setupTime
void setupTime();
// setup Inverter
void setupInv();
// query Inverter
void queryInv();
void readBytes(bool HexOut);
void addCstring(const char* s);
bool goodCheckSum(uint8_t len);
// updateTotal
uint8_t readTotal();
uint8_t updateTotal();
// dirList
void handleDir();
void listFile();
void helpPage();
// web
void handleMetrics();
void handleNotFound();
// minProc
void minProc();
// setupServer
void setupServer();
// setupFS
void setupFS ();
// writeFiles
uint8_t storeData();
uint8_t openFile(const char* s);
void writeTemps();
// read pool,pump and air temperatures
void scan1Wire();
// read values from pool slave
void scan2Wire();
// time scheduled processing
void minProc();
// read last good rain values
void getLastScan();
