#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

const char* ssid = "Alberttt";
const char* password = "iloveyou3000";

const char* serverUrl = "http://192.168.0.38:3000/api/alert";
 
int buzzer = 23;
int led = 12; 
const int soundSensorPin = 35;  
int sda = 21;
int scl = 22;

int soundLevel;

WebServer server(80);

void manualAlertOn() {
  Serial.println("turning on");
  digitalWrite(buzzer, HIGH);
  digitalWrite(led, HIGH);
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"activity\": \"Alert Triggered\", \"soundLevel\": " + String(soundLevel) + "}";

  int httpResponseCode = http.POST(jsonPayload);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server response:"); 
    Serial.println(response);
  } else {
    Serial.println("Error in HTTP request");
  }

  http.end();
  server.send(200, "text/plain", "Alert ON");
}

void manualAlertOff() {
  Serial.println("turning off");
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"activity\": \"Alert Triggered\", \"soundLevel\": " + String(soundLevel) + "}";

  int httpResponseCode = http.POST(jsonPayload);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server response:"); 
    Serial.println(response);
  } else {
    Serial.println("Error in HTTP request");
  }

  http.end();
  server.send(200, "text/plain", "Alert OFF");
}

void setup() {
  Serial.begin(115200);  
  pinMode(soundSensorPin, INPUT);  
  pinMode(buzzer, OUTPUT);         
  pinMode(led, OUTPUT);            

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/alert-on", HTTP_GET, manualAlertOn);
  server.on("/alert-off", HTTP_GET, manualAlertOff);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  soundLevel = analogRead(soundSensorPin);
  Serial.println(soundLevel);  

  if (soundLevel >= 3000) {
    // digitalWrite(buzzer, HIGH);
    Serial.println("buzzer sound");
    digitalWrite(led, HIGH);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonPayload = "{\"activity\": \"Alert Triggered\", \"soundLevel\": " + String(soundLevel) + "}";

      int httpResponseCode = http.POST(jsonPayload);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Server response:"); 
        Serial.println(response);
      } else {
        Serial.println("Error in HTTP request");
      }

      http.end();
    }
    delay(500); 
    digitalWrite(led, LOW);
    digitalWrite(buzzer, LOW);
    delay(500); 
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(led, LOW);
  }

  delay(100);  
}
