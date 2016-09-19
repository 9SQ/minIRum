#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <aJSON.h>

const int send_pin = 12;
const int recv_pin = 14;

const char* ssid = "**********";
const char* password = "**********";
char* localName = "minirum-";
String histIRcode;

ESP8266WebServer webServer(80);
IRsend irsend(send_pin);
IRrecv irrecv(recv_pin);

void handleMessages() {
  if (webServer.method() == HTTP_POST) {
    irrecv.disableIRIn();
    String req = webServer.arg(0);
    char json[req.length() + 1];
    req.toCharArray(json, req.length() + 1);
    aJsonObject* root = aJson.parse(json);
    if (root != NULL) {
      aJsonObject* freq = aJson.getObjectItem(root, "freq");
      aJsonObject* data = aJson.getObjectItem(root, "data");
      const int d_size = aJson.getArraySize(data);
      unsigned int rawData[d_size];
      for (int i = 0; i < d_size; i++) {
        aJsonObject* d_int = aJson.getArrayItem(data, i);
        rawData[i] = d_int->valueint;
      }
      irsend.sendRaw(rawData, d_size, freq->valueint);
      irrecv.enableIRIn();
      req = "";
      aJson.deleteItem(root);
    }
    webServer.send(200, "text/plain", "ok");
  }
  else if (webServer.method() == HTTP_GET) {
    String s = "{\"format\":\"raw\",\"freq\":38,\"data\":[";
    s += histIRcode;
    s += "]}";
    histIRcode = "";
    webServer.send(200, "text/plain", s);
  }
}

void handleNotFound() {
  webServer.send(404, "text/plain", "Not found.");
}

String dumpIRcode (decode_results *results) {
  String s = "";
  for (int i = 1;  i < results->rawlen;  i++) {
    s += results->rawbuf[i] * USECPERTICK;
    if ( i < results->rawlen - 1 ) s += ",";
  }
  return s;
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  irsend.begin();
  irrecv.enableIRIn();
  byte mac[6];
  char buf[6];
  WiFi.macAddress(mac);
  sprintf(buf, "%02x%02x%02x", mac[3], mac[4], mac[5]);
  strcat(localName, buf);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webServer.on("/messages", handleMessages);
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  Serial.println("Web server started.");
  if (MDNS.begin(localName, WiFi.localIP())) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started.");
    Serial.print("http://");
    Serial.print(localName);
    Serial.println(".local");
  }
}

void loop() {
  webServer.handleClient();
  decode_results  results;
  if (irrecv.decode(&results)) {
    histIRcode = dumpIRcode(&results);
    irrecv.resume();
  }
}

