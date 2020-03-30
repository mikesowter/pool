#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

extern ESP8266WebServer server;

void handleDir();
void handleMetrics();
void handleWave();
void handleNotFound();

void setupServer() {
  server.on ( "/", handleMetrics );
  server.on ( "/dir", handleDir );
  server.on ( "/metrics", handleMetrics );
  server.on ( "/wave", handleWave );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println( "server started" );
  if ( MDNS.begin ( "pool" ) ) {
    Serial.print("mDNS responder started: http://pool.local");
  }
}
