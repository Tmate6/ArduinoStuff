// DHT
#include <DHT.h>

#define Type DHT11

int senspin = 15;

float humidity;
float temp;

DHT HT(senspin, Type);

// Wifi
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);
 
StaticJsonDocument<250> jsonDocument;
char buffer[250];

void handlePost() {
  Serial.println("Request recieved");
  Serial.println("Reading sensor");
  
  humidity = HT.readHumidity();
  temp = HT.readTemperature();
  
  server.send(200, "application/json", String("{\"Temp\": " + String(temp) + ", \"Humidity\": " + String(humidity) + "}"));
}

void setup() {
  Serial.begin(115200);

  Serial.println("\nStarting sensor");
  HT.begin();

  Serial.print("Connecting to wifi");
  WiFi.begin("ARRIS-25DF", "1D56D9B1FD31E39C");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, handlePost);
  server.begin();
  Serial.print("Server running, accepting requests.");
}    
       
void loop() {    
  server.handleClient();  
}   
