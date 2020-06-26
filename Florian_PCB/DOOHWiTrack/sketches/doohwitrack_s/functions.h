// This-->tab == "functions.h"

// Expose Espressif SDK functionality
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}
#include <ESP8266WiFi.h>
#include "./structures.h"

#define MAX_APS_TRACKED 100
#define MAX_CLIENTS_TRACKED 1000
extern boolean debug;

beaconinfo aps_known[MAX_APS_TRACKED];                    // Array to save MACs of known APs
int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
clientinfo clients_known[MAX_CLIENTS_TRACKED];            // Array to save MACs of known CLIENTs
int clients_known_count = 0;                              // Number of known CLIENTs
long lastDiscovery = 0;
#define DISCOVERY_DELAY 1500


String formatMac1(uint8_t mac[ETH_MAC_LEN]) {
  String hi = "";
  for (int i = 0; i < ETH_MAC_LEN; i++) {
    if (mac[i] < 16) hi = hi + "0" + String(mac[i], HEX);
    else hi = hi + String(mac[i], HEX);
    if (i < 5) hi = hi + ":";
  }
  return hi;
}

int register_beacon(beaconinfo beacon)
{
  int known = 0;   // Clear known flag
  for (int u = 0; u < aps_known_count; u++)
  {
    if (! memcmp(aps_known[u].bssid, beacon.bssid, ETH_MAC_LEN)) {
      aps_known[u].lastDiscoveredTime = millis();
      aps_known[u].rssi = beacon.rssi;
      known = 1;
      break;
    }   // AP known => Set known flag
  }
  if (! known && (beacon.err == 0))  // AP is NEW, copy MAC to array and return it
  {
    beacon.lastDiscoveredTime = millis();
    memcpy(&aps_known[aps_known_count], &beacon, sizeof(beacon));
    /*    Serial.print("Register Beacon ");
        Serial.print(formatMac1(beacon.bssid));
        Serial.print(" Channel ");
        Serial.print(aps_known[aps_known_count].channel);
        Serial.print(" RSSI ");
        Serial.println(aps_known[aps_known_count].rssi);*/

    aps_known_count++;

    if ((unsigned int) aps_known_count >=
        sizeof (aps_known) / sizeof (aps_known[0]) ) {
      Serial.printf("exceeded max aps_known\n");
      aps_known_count = 0;
    }
  }
  return known;
}

int register_client(clientinfo &ci) {
  int known = 0;   // Clear known flag
  for (int u = 0; u < clients_known_count; u++)
  {
    if (! memcmp(clients_known[u].station, ci.station, ETH_MAC_LEN)) {
      clients_known[u].lastDiscoveredTime = millis();
      clients_known[u].rssi = ci.rssi;
      known = 1;
      break;
    }
  }

  //Uncomment the line below to disable collection of probe requests from randomised MAC's
  //if (ci.channel == -2) known = 1; // This will disable collection of probe requests from randomised MAC's
  
  if (! known) {
    ci.lastDiscoveredTime = millis();
    // search for Assigned AP
    for (int u = 0; u < aps_known_count; u++) {
      if (! memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
        ci.channel = aps_known[u].channel;
        break;
      }
    }
    if (ci.channel != 0) {
      memcpy(&clients_known[clients_known_count], &ci, sizeof(ci));
      if (debug) {Serial.println();
         Serial.print("Register Client ");
         Serial.print(formatMac1(ci.station));
         Serial.print(" Channel ");
         Serial.print(ci.channel);
         Serial.print(" RSSI ");
         Serial.println(ci.rssi);
      }
      clients_known_count++;
    }

    if ((unsigned int) clients_known_count >=
        sizeof (clients_known) / sizeof (clients_known[0]) ) {
      Serial.printf("exceeded max clients_known\n");
      clients_known_count = 0;
    }
  }
  return known;
}


String print_beacon(beaconinfo beacon)
{
  String hi = "";
  if (beacon.err != 0) {
    //Serial.printf("BEACON ERR: (%d)  ", beacon.err);
  } else {
    if (debug) Serial.printf(" BEACON: <=============== [%32s]  ", beacon.ssid);
    if (debug) Serial.print(formatMac1(beacon.bssid));
    if (debug) Serial.printf("   %2d", beacon.channel);
    if (debug) Serial.printf("   %4d\r\n", beacon.rssi);
  }
  return hi;
}

String print_client(clientinfo ci, int wififrame)
{
  String hi = "";
  int u = 0;
  int known = 0;   // Clear known flag
  if (ci.err != 0) {
    // nothing
  } else {
    if (debug) Serial.printf("CLIENT: ");
    if (debug) Serial.print(formatMac1(ci.station));  //Mac of device
    if (debug) Serial.printf(" ==> ");

//    for (u = 0; u < aps_known_count; u++)
//    {
//      if (! memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
//        //       Serial.print("   ");
//        //        Serial.printf("[%32s]", aps_known[u].ssid);   // Name of connected AP
//        known = 1;     // AP known => Set known flag
//        break;
//      }
//    }
//
//    if (! known)  {
//      Serial.printf("   Unknown/Malformed packet \r\n");
//      for (int i = 0; i < 6; i++) Serial.printf("%02x", ci.bssid[i]);
//    } else {
//      //    Serial.printf("%2s", " ");
      
      if (debug) Serial.print(formatMac1(ci.ap));   // Mac of connected AP
      if (debug) Serial.printf("  % 3d", ci.channel);  //used channel
      if (debug) Serial.printf("   % 4d\r\n", ci.rssi);
      //make data a visitor and send it
      String s ="";
      s = s + "02" + ";";
      s = s + formatMac1(ci.ap) + ";";
      s = s + ci.channel + ";";
      s = s + ci.lastDiscoveredTime + ";";
      s = s + ci.rssi + ";";

      if (debug) Serial.print(s);if (debug) Serial.println(wififrame, HEX);
      //Serial.print("Clients registered: ");
      Serial.println(clients_known_count);
//    }
  }
  return hi;
}

void promisc_cb(uint8_t *buf, uint16_t len)
{
  int i = 0;
  uint16_t seq_n_new = 0;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
  } else if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    if ((sniffer->buf[0] == 0x80)) {
      struct beaconinfo beacon = parse_beacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
      if (register_beacon(beacon) == 0) {
        print_beacon(beacon);
        nothing_new = 0;
      }
    } else if ((sniffer->buf[0] == 0x40)) {
          if (millis()- DISCOVERY_DELAY < lastDiscovery){
            //buffer for too fast registrations through just switched on phones
            // ignore it
            if (debug) Serial.println("ignoring ping");} else {
              struct clientinfo ci = parse_probe(sniffer->buf, 36, sniffer->rx_ctrl.rssi);
      //if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci) == 0) {

            lastDiscovery = millis();
          print_client(ci, sniffer->buf[0]);
          nothing_new = 0;
        }
      //}
    }
    }
  } else {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    //Is data or QOS?
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci) == 0) {
          print_client(ci, sniffer->buf[0]);
          nothing_new = 0;
        }
      }
    }
  }
}
