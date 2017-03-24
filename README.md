minIRum
======

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

### See also

回路図とパーツリストはブログ記事を参照してください。

[ミニマルなIRKitクローンを作ってiOSから家電を制御する : Eleclog.](http://eleclog.quitsq.com/2016/09/minirum.html)

## Example Requests

**GET /messages**

```sh
curl -i http://minirum-a492cd.local/messages
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 383
Connection: close
Access-Control-Allow-Origin: *

{"format":"raw","freq":38,"data":[3550,1700,500,400,500,400,500,1300,500,1300,500,400,500,1300,500,400,500,400,500,400,500,1300,500,400,500,400,500,1300,500,400,500,1300,500,400,500,1300,500,400,500,400,500,1300,500,400,500,400,500,400,500,400,500,1300,500,400,500,1300,500,1300,500,400,500,1300,500,400,500,400,500,400,500,400,500,1300,500,400,500,400,500,1300,500,400,500,400,500]}
```

**POST /messages**

```sh
curl -i http://minirum-a492cd.local/messages -d'{"format":"raw","freq":38,"data":[3550,1700,500,400,500,400,500,1300,500,1300,500,400,500,1300,500,400,500,400,500,400,500,1300,500,400,500,400,500,1300,500,400,500,1300,500,400,500,1300,500,400,500,400,500,1300,500,400,500,400,500,400,500,400,500,1300,500,400,500,1300,500,1300,500,400,500,1300,500,400,500,400,500,400,500,400,500,1300,500,400,500,400,500,1300,500,400,500,400,500]}'
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 2
Connection: close
Access-Control-Allow-Origin: *
```

## Requirements

* [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
* [aJson](https://github.com/interactive-matter/aJson)
* [IRremoteESP8266](https://github.com/markszabo/IRremoteESP8266)
