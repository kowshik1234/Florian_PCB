/*
 * contains the code to use the ESP32 WiFi 
 */
 /* WiFi return codes
  *  typedef enum {
    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
} wl_status_t;
  */
 
 #include <WiFi.h>
WiFiClient wificlient;
void config_readWiFi ();
 
String ssid;
String wifipw;
int myLEDpin;
 

 void WIFIstart (int pin) {
 //if (debug == 1) Serial.println("Connecting to WiFi";}
  myLEDpin = pin;
  if (pin >0) {
    digitalWrite (myLEDpin, LOW);
  }
  config_readWiFi();
  if (ssid == "" || wifipw == "") {
    // no wifi access data
  } else {
  WiFi.begin (ssid.c_str(), wifipw.c_str());
  Serial.print ("WiFi connecting, displaying status on pin ");
  Serial.println (pin);

 }
 }

 void WiFistatus () {
  //Serial.print ("WiFi Status: ");
  //Serial.println(WiFi.status());
  /*if (myLEDpin >0) {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite (myLEDpin, HIGH); 
    } else {
    digitalWrite (myLEDpin, LOW);
   }
  }
  */
 }
