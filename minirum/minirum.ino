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
      if (freq != NULL && data != NULL) {
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
        webServer.send(200, "text/plain", "ok");
      } else {
        webServer.send(400, "text/plain", "Invalid JSON format");
      }
    } else {
      webServer.send(400, "text/plain", "Request body is empty");
    }
  }
  else if (webServer.method() == HTTP_GET) {
    String s = "{\"format\":\"raw\",\"freq\":38,\"data\":[";
    s += histIRcode;
    s += "]}";
    histIRcode = "";
    webServer.send(200, "text/plain", s);
  }
}

void handleIndex() {
  String s = "<html lang=\"en\"><head><meta charset=\"utf-8\"/><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>minIRum</title><link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/3.3.7/css/bootstrap.min.css\" /><script src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.min.js\"></script><script src=\"https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/3.3.7/js/bootstrap.min.js\"></script></head><body><div class=\"container\"><div class=\"row\"><div class=\"col-md-12\"><h1>minIRum console <small>";
  s += localName;
  s += ".local</small></h1><p>IP address: ";
  s += String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
  s += "</p><hr><div class=\"form-group\"><textarea class=\"form-control\" id=\"message\" rows=\"10\"></textarea></div><button class=\"btn btn-primary\" id=\"get\">GET</button> <button class=\"btn btn-success\" id=\"post\">POST</button></div></div><script>var xhr = new XMLHttpRequest();var textarea = document.getElementById(\"message\");document.getElementById(\"get\").addEventListener(\"click\", function () {xhr.open('GET', '/messages', true);xhr.onreadystatechange = function() {if(xhr.readyState == 4) {textarea.value =xhr.responseText;}};xhr.send(null);});document.getElementById(\"post\").addEventListener(\"click\", function () {data = textarea.value;xhr.open('POST', '/messages', true);xhr.onreadystatechange = function() {if(xhr.readyState == 4) {alert(xhr.responseText);}};xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');xhr.send(data);});</script></body></html>";
  webServer.send(200, "text/html", s);
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
  webServer.on("/", handleIndex);
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

