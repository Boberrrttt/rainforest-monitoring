#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

const char* ssid = "SKYWORTH-7197";
const char* password = "352191111";

const char* serverUrl = "http://192.168.0.42:3000/api/alert";
 
int buzzer = 23;
int led = 26; 
const int soundSensorPin = 34;  
bool manualMode = false;

int soundLevel;

WebServer server(80);

void manualAlertOn() {
  Serial.println("turning on manually");
  digitalWrite(buzzer, HIGH);
  digitalWrite(led, HIGH);
  manualMode = true;

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonPayload = "{\"activity\": \"Manual Alert\", \"soundLevel\": " + String(soundLevel) + "}";

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
  Serial.println("turning off manually");
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);
  manualMode = false;

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

  if (!manualMode) {
    soundLevel = analogRead(soundSensorPin);
    Serial.println(soundLevel);

    if (soundLevel >= 3000) {
      Serial.println("buzzer sound");
      digitalWrite(buzzer, HIGH);
      digitalWrite(led, HIGH);

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{\"activity\": \"Auto Alert\", \"soundLevel\": " + String(soundLevel) + "}";

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
      
      digitalWrite(buzzer, LOW);
      digitalWrite(led, LOW);
      delay(500);
    } else {
      digitalWrite(buzzer, LOW);
      digitalWrite(led, LOW);
    }
  }

  delay(100);
}
