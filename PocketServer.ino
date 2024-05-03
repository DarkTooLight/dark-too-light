/*
Copyright (c) 2023 cobbsaladluv

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
//Libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
extern "C" {
#include "user_interface.h"
}
//SD
#include <SD.h>
#include <SPI.h>
File myFile;
String data = "";
//Login details
const char* ssid = "SSID";
const char* password = "PASSWORD";
//Initalize the server on HTTP port 80
ESP8266WebServer server(80);
//Other init
String requestURI;
String extension;
//Extension function
String getExtension(String requestURI){
  int i;
  char delimiter[] = ".";
  char *p;
  char string[128];
  String words[2];
  requestURI.toCharArray(string, sizeof(string));
  i = 0;
  p = strtok(string, delimiter);
  while(p && i < 3)
  {
    words[i] = p;
    p = strtok(NULL, delimiter);
    ++i;
  }
  return words[1];
}
//Handler functions
void handler(){
  requestURI = server.uri();
  if(SD.exists(requestURI)){
    myFile = SD.open(requestURI);
    {
      if(myFile){
        while(myFile.available()){
          data+=(char)myFile.read();
          yield();
        }
        myFile.close();
        extension = getExtension(requestURI);
        if(extension == "html"){
          server.send(200,"text/html",data);
        }
        else if(extension == "txt"){
          server.send(200,"text/plain",data);
        }
        else if(extension == "js"){
          server.send(200,"text/javascript",data);
        }
        else if(extension == "css"){
          server.send(200,"text/css",data);
        }
        else if(extension == "bmp"){
          server.send(200,"image/bmp",data);
        }
        else if(extension == "png"){
          server.send(200,"image/png",data);
        }
        else if(extension == "exe"){
          server.send(200, "application/vnd.microsoft.portable-executable",data);
        }
        else if(extension == "jpeg" || extension == "jpg"){
          server.send(200, "image/jpeg",data);
        }
        else{
          server.send(200,"text/plain",data);
        }
        data="";
      }
    }
  }
  else{
    server.send(404,"text/plain","404 Not Found.");
  }
}
void statusHandler(){
  server.send(200,"text/plain","PocketServer v0.1 is online.");
}
void memInfo(){
  Serial.print("Available memory: ");
  Serial.print(system_get_free_heap_size());
  Serial.println(" bytes");
  server.send(200,"text/plain","Memory info sent to serial comms.");
}
void indexHandler(){
  if(SD.exists("index.html")){
    myFile = SD.open("index.html");
    if(myFile){
      while(myFile.available()){
        data+=(char)myFile.read();
      }
      myFile.close();
      server.send(200,"text/html",data);
      data="";
    }
  }
  else{
    server.send(404,"text/plain","404 Not Found (no index.html file)");
  }
}
//Setup
void setup(void){
  //Init serial connection
  Serial.begin(9600);
  //Boot message
  Serial.println("");
  Serial.println("PocketServer v0.2 Booting...");
  //SD
  Serial.println("Initializing SD card...");
  if(!SD.begin(4)){
    Serial.println("Initialization failed, exiting.");
    return;
  }
  Serial.println("SD card init success.");
  //Connect to wireless network
  WiFi.begin(ssid, password);
  Serial.println("Connecting to wireless network. If this does not load check SSID and password.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to network: ");
  Serial.println(ssid);
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  //Handle requests by treating everything as a Not Found, then returning data from there. 
  server.onNotFound(handler);
  //Add a status page handler
  server.on("/status",statusHandler);
  //Add an index page hander
  server.on("/",indexHandler);
  //Add a memory info dump trigger
  server.on("/meminfo",memInfo);
  //Start the server
  server.begin();
  Serial.println("Started server.");
}
//Loop
void loop(void){
  server.handleClient();
}
