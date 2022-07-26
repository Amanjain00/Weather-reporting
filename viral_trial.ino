#include "DHT.h"
#include <WiFi.h>
#include <Wire.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP085.h>
#include <HTTPClient.h>
#define DHT11PIN 16

Adafruit_BMP085 bmp;

const char *SSID = "RealAman";
const char *PWD = "amanjain";
WebServer server(80);

const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "T4DDTA5I9WURLIZ1";
  
DHT dht(DHT11PIN, DHT11);

const int led = 2;

unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 10000;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
/* Start the DHT11 Sensor */
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }
  dht.begin();

  Serial.print("Connecting to Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();

  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  float pres = bmp.readPressure();
  float alt = bmp.readAltitude();

  Serial.print("Temperature: ");
  Serial.println(temp);
  
  Serial.print("Humidity: ");
  Serial.println(humi);
  
  Serial.print("Pressure = ");
  Serial.print(pres);
  Serial.println(" Pa");
    
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(alt);
  Serial.println(" meters");

  Serial.print("------------------------------------------------");
  Serial.println("------------------------------------------------");
      // Your Domain name with URL path or IP address with path
      if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData1 = "api_key=" + apiKey + "&field1=" + String(temp);           
      // Send HTTP POST request
      int httpResponseCode1 = http.POST(httpRequestData1);
     
      Serial.print("HTTP Response code for Temperature: ");
      Serial.println(httpResponseCode1);
//-------------------------------------------------------------------------
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      String httpRequestData2 = "api_key=" + apiKey + "&field2=" + String(humi);           
      // Send HTTP POST request
      int httpResponseCode2 = http.POST(httpRequestData2);
     
      Serial.print("HTTP Response code for Humidity: ");
      Serial.println(httpResponseCode2);
//-----------------------------------------------------------------------------------
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData3 = "api_key=" + apiKey + "&field3=" + String(pres);           
      // Send HTTP POST request
      int httpResponseCode3 = http.POST(httpRequestData3);

      Serial.print("HTTP Response code for Pressure: ");
      Serial.println(httpResponseCode3);
//--------------------------------------------------------------------------------
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      String httpRequestData4 = "api_key=" + apiKey + "&field4=" + String(alt);           
      // Send HTTP POST request
      int httpResponseCode4 = http.POST(httpRequestData4);
     
      Serial.print("HTTP Response code for Altitude: ");
      Serial.println(httpResponseCode4);
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

 delay(5000);
}
