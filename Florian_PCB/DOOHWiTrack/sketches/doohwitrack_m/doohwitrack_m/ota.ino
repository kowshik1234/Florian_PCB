//OTA HTTP Update
extern const int SV;
extern WiFiClient wificlient;
#include <HTTPClient.h>
#include <HTTPUpdate.h>
const char* fwUrlBase = "http://doohboxx.com/doohwitrack/updates/dsc/";

void checkForOTAUpdates() {
  String fwVersionURL = String( fwUrlBase );
  fwVersionURL.concat( "version.txt" );

  Serial.println( "Checking for firmware updates." );
  Serial.println( "Firmware version URL: " );
  Serial.println( fwVersionURL );
  String fwImageURL = String (fwUrlBase);

  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version: " );
    Serial.println( SV );
    Serial.print( "Available firmware version: " );
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if( newVersion > SV ) {
      Serial.println( "Preparing to update" );
      fwImageURL.concat( "doohwitrack_m.ino.esp32.bin" );
      t_httpUpdate_return ret = httpUpdate.update(wificlient, fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    }
    else {
      Serial.println( "Already on latest version" );
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  httpClient.end();
}
