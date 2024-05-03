/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-microsd-card/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Replace with your network credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* PARAM_MESSAGE = "file";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void initSDCard(){
  if(!SD.begin(4)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initSDCard();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String message;
    String ftype;
    if (request->hasParam(PARAM_MESSAGE)) {
        message = request->getParam(PARAM_MESSAGE)->value();
        message.replace('~', '/');
    }
    if (request->hasParam("ftype")) {
        ftype = request->getParam("ftype")->value();
        ftype.replace('~', '/');
    }
    request->send(SD, "/"+message, ftype);
    Serial.printf("User requested file '%s' using file type '%s'\n", message, ftype);
  });

  server.serveStatic("/", SD, "/");

  server.begin();
}

void loop() {
  delay(20);
}