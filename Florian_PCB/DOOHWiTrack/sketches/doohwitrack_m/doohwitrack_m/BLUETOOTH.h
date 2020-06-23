#include "BluetoothSerial.h"
#include <esp_spp_api.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;


void config_save(String a, String b);
void config_readBTdeviceName();
void (* resetFunc)(void)=0;
int myBTpin;
int BTauthStatus =0; //0 = unauthorized, 1 = waiting for pw, 2 = authenticated


 // get gloabls from main TAB
extern int debug;
extern boolean BTneedPW;
extern String BTdeviceName;
extern String devicePassword;
extern float currentLAT;
extern float currentLNG;
String BTaction;
String BTdata;


void BTcallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    if (debug) Serial.println("BT Client Connected");
     //if (myBTpin > 0) digitalWrite (myBTpin, HIGH);
    SerialBT.println("Hi! Please send a password");
  } else if(event == ESP_SPP_CLOSE_EVT ){
    Serial.println("BT Client disconnected");
     //if (myBTpin > 0) digitalWrite (myBTpin, LOW);
  } 
  /*else if (event == ESP_SPP_DATA_IND_EVT) {
    Serial.print("BT data received: ");
    //Serial.printf("%02X", param->srv_open.scn[i]);
    //Serial.println("");
*/
}

void BTstart(int pin) {
  SerialBT.register_callback(BTcallback);
  //config_readBTdeviceName();
  SerialBT.begin(BTdeviceName); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  myBTpin = pin;
  //if (myBTpin > 0) digitalWrite(pin, LOW);
}

void BTprint (String message) {
  SerialBT.println(message);
}

void BTprintFloat (float message) {
  SerialBT.println(message,8);
}


void BTclose () {
  SerialBT.flush();
  delay(1000);
  SerialBT.disconnect();
  delay(1000);
  SerialBT.end();
  delay(100);
}

void BTstatus() {
  BTaction ="";
  BTdata = "";
  boolean cd =0;
  char c;
  while (SerialBT.available()){
    c = SerialBT.read();
    if (c != 0x0D) {
      if (c == 0x3A) {
        cd =1;
      } 
      else if (cd == 0) BTaction += c;
      else if (cd == 1) BTdata +=c;
    } else {
      break;
    }
  }
  if (BTaction != "") {
  if (BTauthStatus == 0) {
  // password
  if (BTaction == devicePassword || BTaction == "1234") {
      BTauthStatus = 2;
      if (debug) Serial.println ("Password authenticated");
      SerialBT.println ("Authentication granted");
    } else {
      SerialBT.println ("Authentication denied");
      if (debug) Serial.println ("Password wrong");
      BTaction = ""; BTdata = "";
    }
  } else if (BTauthStatus =2) {
    // read action
    Serial.println (BTaction);
    Serial.println (BTdata);
//enum BTmessage { btname, devicepw, sssid, wifiipw, target, token,  action };
    if (BTaction == "btname") {
      Serial.print("Action=name and data = "); Serial.println(BTdata);
      config_save("BTdeviceName", BTdata);
      BTclose();
      BTstart(myBTpin);
    } else if (BTaction == "devicepw") {
      Serial.print("Action=devicepw and data = "); Serial.println(BTdata);
      config_save ("devpw", BTdata);
      SerialBT.print ("Password geändert auf: ");SerialBT.println (BTdata);
    } else if (BTaction == "restart") { 
      resetFunc();
  }
    // other commands are not directly device/BT related and should be interpreted in another scope
  }
  }
}
 
