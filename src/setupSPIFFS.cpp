#include <LittleFS.h>

extern FSInfo fs_info;
extern File fd,fe;

void setupLittleFS () {

//   if(!LittleFS.format()) Serial.println("LittleFS.format failed");

  if ( !LittleFS.begin() ) {
    if ( !LittleFS.format() ) Serial.println("LittleFS.format failed");
    Serial.println("LittleFS.begin failed");
  }
 
  LittleFS.info(fs_info);
  Serial.print(fs_info.totalBytes);
  Serial.println(" bytes available");
  Serial.print(fs_info.usedBytes);
  Serial.println(" bytes used:");

  fd = LittleFS.open("/diags.txt","a");
  fe = LittleFS.open("/errmess.txt","a");
}
