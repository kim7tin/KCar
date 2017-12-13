/*
   WebSocketServer_LEDcontrol.ino

    Created on: 26.11.2015

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <ArduinoOTA.h>
#include "page.h"


#define LIGHT 2
#define RWA 14
#define RWB 12
#define LWA 13
#define LWB 15

#define RUA 16
#define RUB 0
#define RUS 4
#define RDS 5


ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

boolean roboUp = false;
page page;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      char command[5];
      boolean control;
      memcpy(&command, payload, 5);
      control = payload[5];


      Serial.println(command);
      Serial.println((byte)control);
      // Light control
      if (memcmp(command, "LIGHT", 5) == 0) {
        if (control == '0') {
          digitalWrite(LIGHT, LOW);
        } else if (control == '1') {
          digitalWrite(LIGHT, HIGH);
        }
      }

      if (memcmp(command, "RIWHE", 5) == 0) {
        if (control == 'F') {
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, HIGH);
        } else if (control == 'B') {
          digitalWrite(RWA, HIGH);
          digitalWrite(RWB, LOW);
        } else if (control == 'S') {
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, LOW);
        }
      }

      if (memcmp(command, "LEWHE", 5) == 0) {
        if (control == 'F') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, HIGH);
        } else if (control == 'B') {
          digitalWrite(LWA, HIGH);
          digitalWrite(LWB, LOW);
        } else if (control == 'S') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, LOW);
        }
      }


      if (memcmp(command, "UPSTA", 5) == 0) {
        if (control == '1') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, HIGH);
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, HIGH);
        } else if (control == '0') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, LOW);
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, LOW);
        }
      }

      if (memcmp(command, "DOSTA", 5) == 0) {
        if (control == '1') {
          digitalWrite(LWA, HIGH);
          digitalWrite(LWB, LOW);
          digitalWrite(RWA, HIGH);
          digitalWrite(RWB, LOW);
        } else if (control == '0') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, LOW);
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, LOW);
        }
      }

      if (memcmp(command, "RISTA", 5) == 0) {
        if (control == '1') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, HIGH);
          digitalWrite(RWA, HIGH);
          digitalWrite(RWB, LOW);
        } else if (control == '0') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, LOW);
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, LOW);
        }
      }

      if (memcmp(command, "LESTA", 5) == 0) {
        if (control == '1') {
          digitalWrite(LWA, HIGH);
          digitalWrite(LWB, LOW);
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, HIGH);
        } else if (control == '0') {
          digitalWrite(LWA, LOW);
          digitalWrite(LWB, LOW);
          digitalWrite(RWA, LOW);
          digitalWrite(RWB, LOW);
        }
      }


      if (memcmp(command, "ROBUP", 5) == 0) {
        if (control == 'U') {
          digitalWrite(RUA, HIGH);
          digitalWrite(RUB, LOW);
          roboUp = true;
        } else if (control == 'D') {
          digitalWrite(RUA, LOW);
          digitalWrite(RUB, HIGH);
          roboUp = false;
        }
      }



      break;
  }

}

void setup() {
  //USE_SERIAL.begin(921600);
  Serial.begin(115200);

  //USE_SERIAL.setDebugOutput(true);

  pinMode(LIGHT, OUTPUT);
  pinMode(RWA, OUTPUT);
  pinMode(RWB, OUTPUT);
  pinMode(LWA, OUTPUT);
  pinMode(LWB, OUTPUT);

  pinMode(RUA, OUTPUT);
  pinMode(RUB, OUTPUT);
  pinMode(RUS, INPUT_PULLUP);
  pinMode(RDS, INPUT_PULLUP);


  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFiMulti.addAP("KHome", "HoangTung");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // handle index
  server.on("/", []() {
    // send index.html
    server.send(200, "text/html", page.index());
  });

  server.on("/index.js", []() {
    server.send(200, "script", page.js());
  });

  server.on("/index.css", []() {
    server.send(200, "stylesheet", page.css());
  });

  server.begin();

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);


  ArduinoOTA.setHostname("KCar1");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void loop() {
  webSocket.loop();
  server.handleClient();
  ArduinoOTA.handle();

  if (roboUp && !digitalRead(RUS)) {
    digitalWrite(RUA, LOW);
    digitalWrite(RUB, LOW);
  }

  if (!roboUp && !digitalRead(RDS)) {
    digitalWrite(RUA, LOW);
    digitalWrite(RUB, LOW);
  }
}

