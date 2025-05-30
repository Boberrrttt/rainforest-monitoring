#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "SKYWORTH-7197";
const char* password = "352191111";

const char* serverUrl = "http://192.168.0.42:3000/api/alert";

// Pin configuration
int buzzer = 23;
int led = 12; 
const int soundSensorPin = 34;  
int sda = 21;
int scl = 22;

int soundLevel;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

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

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Monitoring...");
}

void loop() {
  soundLevel = analogRead(soundSensorPin);
  Serial.println(soundLevel);  

  if (soundLevel >= 4095) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Warning:        "); 
    lcd.setCursor(0, 1);
    lcd.print("Illegal Activity");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"activity\": \"Alert Triggered\", \"soundLevel\": " + String(soundLevel) + ", \"timestamp\": " + String(millis()) + "}";

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
    lcd.setCursor(0, 0);
    lcd.print("Monitoring...   "); 
    lcd.setCursor(0, 1);
    lcd.print("                "); 
  }

  delay(100);  
}


