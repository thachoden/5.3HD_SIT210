#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char ssid[] = "Quang Huy";
const char pass[] = "Danang040903";

// Firebase host
const char firebaseHost[] = "trafficlightcontrol-13c4b-default-rtdb.asia-southeast1.firebasedatabase.app";

// Firebase path to read 
const char firebasePath[] = "/trafficLight.json";

// HTTPS port
const int httpsPort = 443;

// Pins for LEDs
const int redPin = 2;
const int greenPin = 3;
const int bluePin = 4;

WiFiSSLClient wifiClient;// Secure WiFi client for HTTPS
HttpClient httpClient = HttpClient(wifiClient, firebaseHost, httpsPort);

void setup() {
  Serial.begin(115200);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);

  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void reconnect() {
  if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
    }
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
}
void toggleLED(int pin, bool state) {
  digitalWrite(pin, state ? HIGH : LOW);
}

void loop() {
  Serial.println("Requesting data from Firebase...");

  // Send GET request to Firebase REST API
  httpClient.get(firebasePath);

  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  if (statusCode == 200) {
    Serial.println("Received data:");
    Serial.println(response);

    // Parse JSON response
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error) {
      //Example: read boolean values for red, green, blue
      bool redState = doc["red"] | false;
      bool greenState = doc["green"] | false;
      bool blueState = doc["blue"] | false;

      toggleLED(redPin, redState);
      toggleLED(greenPin, greenState);
      toggleLED(bluePin, blueState);
    } else {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.print("Error fetching data, status code: ");
    Serial.println(statusCode);
    reconnect();
  }

  delay(1000);  // Wait 1 second before next request
}
