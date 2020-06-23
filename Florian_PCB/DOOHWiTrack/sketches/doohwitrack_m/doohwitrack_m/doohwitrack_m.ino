// DOOHBOXX DOOHwiTrack
// OTA HTTP Update
const int SV = 1; // sketch version
#include "./structures.h"
//
#include "./WiFi32.h"
#include "./BLUETOOTH.h"

// common globals
int debug = 1;
const int PINblt =0;
const int PINwifi = 0;
String devicePassword;
unsigned long millisCounter;
extern String records [19][2];

// Bluetooth globals
String BTdeviceName;

void evaluateBTcommand() {
  //Serial.println ("action 0-6 = " + BTaction.substring(0,6));
  if (debug) Serial.print (BTaction); Serial.print(":"); Serial.println(BTdata);
  if (BTaction == "ssid") {
      if (debug) Serial.print("Command= SSID and data = "); Serial.println(BTdata);
      config_save (BTaction, BTdata);
      SerialBT.println("SSID changed to " + BTdata);
    } else if (BTaction == "wifipw") {
      if (debug) Serial.print("Command= WIFI pw and data = "); Serial.println(BTdata);
      config_save (BTaction, BTdata);    
      SerialBT.println("WiFi password changed to " + BTdata);
      SerialBT.println("Connecting to WiFi...");
      WIFIstart(0);
    } else if (BTaction == "clp_") {
      //clear the preferences
      Serial.println("Clearing the preferences");
      SerialBT.println("Clearing the preferences");
      config_clear();
    } else if (BTaction.substring(0,6) == "record") {
      //save the record
    } else if (BTaction.substring(0,7) == "-record") {
      //delete the record
      config_delete (BTaction.substring(1,9));  
    } else if (BTaction == "reset1") {
      //reset ESP8266 at Pin 27
    } else if (BTaction == "reset2") {
      //reset ESP8266 at Pin 26
     } else if (BTaction == "ota") {
      //reset ESP8266 at Pin 26 
      checkForOTAUpdates();        
    } else {
    //command unknown
    Serial.println ("Command unknown");
  }
  }

void setup() {
  delay(3000);
  debug = 1;
  Serial.begin(9600);
  Serial.println("hi");
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  Serial2.begin(9600, SERIAL_8N1, 18, 19);
  //pinMode (PINblt, OUTPUT);
  //pinMode (PINwifi, OUTPUT);
  millisCounter=millis();
  config_setup();
  WIFIstart(0);
  BTstart(PINblt);
}

void loop() {
  if (millis()-millisCounter >= 2000){
  millisCounter=millis();
  WiFistatus();
  //--check for new BT command
  BTstatus();
  if (BTaction != "") evaluateBTcommand();
    while (Serial1.available() >0) {
    Serial.write(Serial1.read());;
  }
    while (Serial2.available() >0) {
    Serial.write(Serial2.read());;
  }
  }
}
