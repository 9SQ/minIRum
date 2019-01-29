#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <aJSON.h>

#define SEND_PIN 12
#define RECV_PIN 14
#define TIMEOUT kMaxTimeoutMs
#define CAPTURE_BUFFER_SIZE 1024

char* localName = "minirum-";
String ssid = "";
String pass = "";
String histIRcode;
boolean settingMode;
DNSServer dnsServer;
const IPAddress apIP(192, 168, 1, 1);
const char* apSSID = "minIRum_SETUP";
String ssidList;

ESP8266WebServer webServer(80);
IRsend irsend(SEND_PIN);
IRrecv irrecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT, true);

String dumpIRcode (decode_results *results) {
  String s = "";
  for (int i = 1;  i < results->rawlen;  i++) {
    s += results->rawbuf[i] * RAWTICK;
    if ( i < results->rawlen - 1 ) s += ",";
  }
  return s;
}

void startWebServer() {
  if (settingMode) {
    Serial.println(WiFi.softAPIP());
    webServer.on("/", handleSettings);
    webServer.on("/setap", handleSetAP);
    webServer.onNotFound(handleSettings);
  }
  else {
    if (MDNS.begin(localName, WiFi.localIP())) {
      MDNS.addService("http", "tcp", 80);
      Serial.println("MDNS responder started.");
      Serial.print("http://");
      Serial.print(localName);
      Serial.println(".local");
    }
    webServer.on("/", handleIndex);
    webServer.on("/messages", handleMessages);
    webServer.on("/reset", handleReset);
    webServer.onNotFound(handleNotFound);
  }
  webServer.begin();
  Serial.println("Web server started.");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  if (restoreConfig()) {
    if (checkConnection()) {
      settingMode = false;
      irsend.begin();
      irrecv.enableIRIn();
      byte mac[6];
      char buf[6];
      WiFi.macAddress(mac);
      sprintf(buf, "%02x%02x%02x", mac[3], mac[4], mac[5]);
      strcat(localName, buf);
      startWebServer();
      return;
    }
  }
  settingMode = true;
  setupMode();
}

void loop() {
  if (settingMode) {
    dnsServer.processNextRequest();
  }
  webServer.handleClient();
  decode_results  results;
  if (irrecv.decode(&results)) {
    histIRcode = dumpIRcode(&results);
    irrecv.resume();
  }
}

