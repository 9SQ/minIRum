void handleSettings() {
  String s = "<h1>minIRum Wi-Fi Settings</h1><p>Please enter your password by selecting the SSID.</p>";
  s += "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">";
  s += ssidList;
  s += "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"></form>";
  webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
}

void handleSetAP() {
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  String ssid = urlDecode(webServer.arg("ssid"));
  Serial.print("SSID: ");
  Serial.println(ssid);
  String pass = urlDecode(webServer.arg("pass"));
  Serial.print("Password: ");
  Serial.println(pass);
  Serial.println("Writing SSID to EEPROM...");
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(i, ssid[i]);
  }
  Serial.println("Writing Password to EEPROM...");
  for (int i = 0; i < pass.length(); ++i) {
    EEPROM.write(32 + i, pass[i]);
  }
  EEPROM.commit();
  Serial.println("Write EEPROM done!");
  String s = "<h1>Setup complete.</h1><p>device will be connected to \"";
  s += ssid;
  s += "\" after the restart.";
  webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
  ESP.restart();
}

void handleReset() {
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  String s = "<h1>Wi-Fi settings was reset.</h1><p>Please restart device.</p>";
  webServer.send(200, "text/html", makePage("Reset Wi-Fi Settings", s));
}

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
        const uint16_t d_size = aJson.getArraySize(data);
        uint16_t rawData[d_size];
        for (int i = 0; i < d_size; i++) {
          aJsonObject* d_int = aJson.getArrayItem(data, i);
          rawData[i] = d_int->valueint;
        }
        irsend.sendRaw(rawData, d_size, (uint16_t)freq->valueint);
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
  s += "</p><hr><div class=\"form-group\"><textarea class=\"form-control\" id=\"message\" rows=\"10\"></textarea></div><button class=\"btn btn-primary\" id=\"get\">GET</button> <button class=\"btn btn-success\" id=\"post\">POST</button><hr><p><a href=\"/reset\">Reset Wi-Fi Settings</a></p></div></div><script>var xhr = new XMLHttpRequest();var textarea = document.getElementById(\"message\");document.getElementById(\"get\").addEventListener(\"click\", function () {xhr.open('GET', '/messages', true);xhr.onreadystatechange = function() {if(xhr.readyState == 4) {textarea.value =xhr.responseText;}};xhr.send(null);});document.getElementById(\"post\").addEventListener(\"click\", function () {data = textarea.value;xhr.open('POST', '/messages', true);xhr.onreadystatechange = function() {if(xhr.readyState == 4) {alert(xhr.responseText);}};xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');xhr.send(data);});</script></body></html>";
  webServer.send(200, "text/html", s);
}

void handleNotFound() {
  webServer.send(404, "text/plain", "Not found.");
}
