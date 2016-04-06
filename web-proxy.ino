#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "";
const char* password = "";

const char* newssid = "";
const char* newpassword = "";

ESP8266WebServer server(80);

void handleRoot() 
{
  server.send(200, "text/plain", "Enter the url");
}

void handleNotFound()
{ const char* host;
  WiFiClient client;

  Serial.print("Requesting uri");
  String requestUri = server.uri();
  Serial.println(requestUri);
  
  requestUri.remove(0,1);
  int i = requestUri.indexOf('/');
  String domain = requestUri.substring(0,i);
  host = domain.c_str();
  requestUri.remove(0,i);

  Serial.print("Host: ");
  Serial.println(host);
  Serial.println(requestUri);

  while (!client.connect(host, 80)) 
  {
    Serial.println("connection failed, Trying again");
  }
  
  client.print(String("GET ") + requestUri);
  
  client.print(String(" HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
  while(!client.available())
  {
    yield();
  }

  String line;
  while(client.available())
  {
    line = client.readStringUntil('\r');
    line.replace("HTTP", "WHAT-IS-THAT");
    line.replace("http", "WHAT-IS-THAT");
  }
  server.send(200, "text/html", line);

  client.stop();
}

void setup(void)
{
  Serial.begin(115200);
  for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
  
  WiFi.softAP(newssid, newpassword);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "It works");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");
  WiFiMulti.addAP(ssid, password);
}

void loop(void){
  server.handleClient();
}
