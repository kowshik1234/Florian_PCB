/* classes to read and write a config file on ESP32
 *  2020
 */
/* STORAGE BYTES
 *  0-9 Name of device
 *  10-17 Device password
 *  18-37 WiFi Name (SSID)
 *  38-49 WiFi Password
 *  50-129 REST target
 *  130-161 Security Token
 *  162-181 param name 1
 *  182-201 param name 2
 *  202-221 param name 3
 *  
 *  position and paramters = action
 *  a position consists of 2 x 4 bytes =8. for the 3 parameter we allow a max of 7 chars
 *  total bytes for one action = 2 x 4 + 3 x7 = 29 bytes
 *  
 *  222-250 action 1
 *  251-279 action 2
 *  ...
 *  action= floatN;floatE;paramValue
 */

#include <EEPROM.h>
#include <Preferences.h>
Preferences cconfig;
String locName [10];
float locGPS [10][2];
String records [19][2];


// get gloabls from main TAB
extern int debug;
extern String BTdeviceName;
extern String devicePassword;
extern String ssid;
extern String wifipw;

void config_clear () {
  cconfig.clear();
}

void config_delete (String recName){
  Serial.println ("Deleting record " + recName);
  int recNameL = recName.length()+1;
  char recIndexName[recNameL];
  recName.toCharArray(recIndexName,recNameL);
  cconfig.remove(recIndexName);
}

void config_readRecords() {
  String rec;
  String recName;
  String recContent;
  int recCount = 0;

  // read how many records exist
  //then read them all and put them in an array
  for (int f=0 ; f < 20 ; f++) {
    // put only existing records to the array
    recName = "record" + String(f);
    int l = 8 + (f>9);
    char recIndexName[l];
    recName.toCharArray(recIndexName,l);
    
    recContent = cconfig.getString(recIndexName);
    if (recContent != "") {
      records[recCount][0]= recName;
      records[recCount][1]= recContent;
      Serial.print(recName + ":");Serial.println(records[recCount][1]);
      recCount +=1;
    } else {
      //this record does not exist, keep looping
    }
  }
  Serial.println ("Read records: " + recCount);
}

void config_readWiFi () {
  ssid = cconfig.getString("ssid");
  if (debug) Serial.print ("config SSID: "); Serial.println (ssid);
  wifipw = cconfig.getString("wifipw");
  if (debug) Serial.print ("config WIFI pw: "); Serial.println (wifipw);
}

void config_readBTdeviceName () {
  BTdeviceName = cconfig.getString("BTdeviceName");
  if (BTdeviceName == "") BTdeviceName = "DOOHBOXX LOCATION SCOUT";
  if (debug) Serial.println (BTdeviceName);
}

void config_readDevicePassword () {
  devicePassword = cconfig.getString("devpw");
  if (debug) Serial.print ("Passwort read from config: "); Serial.println (devicePassword);
  if (devicePassword == "") devicePassword = "1234";
}

void config_setup () {
  cconfig.begin("doohboxx",false);
  cconfig.clear();
  config_readBTdeviceName();
  config_readDevicePassword();
  config_readRecords();
}

void config_save (String action, String ddata) {
  Serial.println ("config save called with identifier " + action + " and data = " + ddata);
  int actionL = action.length()+1;
  char cAction[actionL];
  action.toCharArray(cAction, actionL);
  Serial.println(cAction);
  cconfig.putString(cAction,ddata);
}
