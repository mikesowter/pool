#include <fs.h>

extern FSInfo fs_info;
extern File fd,fe;

void setupSPIFFS () {

//   if(!SPIFFS.format()) Serial.println("SPIFFS.format failed");

  if(!SPIFFS.begin()) Serial.println("SPIFFS.begin failed");
 
  SPIFFS.info(fs_info);
  Serial.print(fs_info.totalBytes);
  Serial.println(" bytes available");
  Serial.print(fs_info.usedBytes);
  Serial.println(" bytes used:");

  fd = SPIFFS.open("/diags.txt","a");
  fe = SPIFFS.open("/errmess.txt","a");
}
