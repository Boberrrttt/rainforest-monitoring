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
const int soundSensorPin = 34;  // Use GPIO 34 for analog input
int sda = 21;
int scl = 22;

int soundLevel;

// LCD configuration (adjust address if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows

void setup() {
  Serial.begin(115200);  // Initialize serial communication at 115200 baud rate
  pinMode(soundSensorPin, INPUT);  // Set the sound sensor pin as input
  pinMode(buzzer, OUTPUT);         // Set buzzer pin as output
  pinMode(led, OUTPUT);            // Set LED pin as output

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
  // Read analog value from the sound sensor
  soundLevel = analogRead(soundSensorPin);

  // Print the sound level to the Serial Monitor and Plotter
  Serial.println(soundLevel);  // Only the value, no extra text for better plotting

  // Check if the sound level reaches the threshold
  if (soundLevel >= 4095) {
    // Trigger the buzzer and blink the LED
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Warning:        "); // Clear first row
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
    delay(500); // Blink duration
    digitalWrite(led, LOW);
    digitalWrite(buzzer, LOW);
    delay(500); // Off duration
  } else {
    // Turn off buzzer and LED
    digitalWrite(buzzer, LOW);
    digitalWrite(led, LOW);
    lcd.setCursor(0, 0);
    lcd.print("Monitoring...   "); // Reset display
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear second row
  }

  // Add a small delay to stabilize readings
  delay(100);  // Reduce delay for smoother plotting
}



// #include <WiFi.h>
// #include <HTTPClient.h>

// // Replace with your WiFi credentials
// const char* ssid = "SKYWORTH-7197";
// const char* password = "352191111";

// // Replace with your local machine's IP and port
// const char* serverUrl = "http://192.168.0.42:3000/api/alert";

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected to WiFi");

//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;
//     http.begin(serverUrl);
//     http.addHeader("Content-Type", "application/json");

//     // Create a JSON payload
//     String jsonPayload = "{\"type\": \"gunshot\", \"timestamp\": " + String(millis()) + "}";

//     int httpResponseCode = http.POST("{\"message\": \"Hi web app\"}");

//     Serial.print("HTTP Response code: ");
//     Serial.println(httpResponseCode);

//     if (httpResponseCode > 0) {
//       String response = http.getString();
//       Serial.println("Server response:");
//       Serial.println(response);
//     } else {
//       Serial.println("Error in HTTP request");
//     }

//     http.end();
//   }
// }

// void loop() {
//   // Empty loop
// }
