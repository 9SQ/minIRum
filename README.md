#minIRum

A minimal implementation of infrared sender/receiver like IRKit by ESP8266

## Description

IRKitの GET,POST /messages 同様の機能**だけ**をESP8266で実現する最小限の実装です

ESP-WROOM-02で動作確認済み

Maybe it will work on other ESP* boards.

## How to use

以下の通り接続

* GPIO 12 - Infrared LED
* GPIO 14 - Infrared receiver

赤外線リモコン受信モジュールは[OSRB38C9AA](http://akizukidenshi.com/catalog/g/gI-04659/)などの3.3V駆動可能のものを利用します

以下の2か所を適宜変更

```C
const char* ssid = "**********"; // Wi-FiアクセスポイントのSSID
const char* password = "**********"; // パスワード
```

## Requirements

* [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
* [aJson](https://github.com/interactive-matter/aJson)
* [IRremoteESP8266](https://github.com/markszabo/IRremoteESP8266)