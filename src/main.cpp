#include <Arduino.h>
#include "WiFiManager.h"
#include "OTAUpdateServer.h"
#include <WebServer.h>
#include <Credentials.h>
#include "BoatSimulator.h"
#include "GpsCheck.h"

#include "BluetoothSerial.h" // Include Bluetooth library

BluetoothSerial SerialBT;

// OTA server configuration
const char *host = "esp32-raw-gps";

// Define GPIO pin to monitor
const int enableOtaPin = 25; // Replace with the actual pin you are using
const int enableRealGPS = 26; // Replace with the actual pin you are using

unsigned long lastNMEATime = 0;

// Create objects
WiFiManager wifiManager(wifiCredentials, sizeOfWifiCredentials);
WebServer server(80); // Shared WebServer object
OTAUpdateServer otaUpdateServer(server);
GpsCheck gpsCheck;
BoatSimulator boat(57.57367, 11.92692); // San Francisco coordinates

bool otaEnabled = false; // Track OTA state

void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); // Turn the LED on

  // // Setup GPIO pin mode
  pinMode(enableOtaPin, INPUT);
  pinMode(enableRealGPS, INPUT);

  if (digitalRead(enableOtaPin) == HIGH)
  {
    Serial.println("Enabling OTA mode...");
    // Setup WiFi
    wifiManager.setup(host);
    otaUpdateServer.setup();

    // Start the shared server
    server.begin();
    otaEnabled = true;
    Serial.println("Enabled OTA mode");
  }
  else
  {
    Serial.println("Enabling NMEA mode...");
    boat.setup();
    if (digitalRead(enableRealGPS) == HIGH)
    {
      SerialBT.begin("ESP32_GPS"); 
      if (SerialBT.connect("Garmin GLO 2 #38ad1")) { // Replace with your GPS module's name
        Serial.println("Connected to GPS module!");
      } else {
        Serial.println("Failed to connect. Make sure the GPS module is on.");
        while (1); // Stop the program
      }
    }

  }
}

void loop()
{
  digitalWrite(2, LOW); // Turn the LED on

  if (digitalRead(enableOtaPin) == !otaEnabled) // Restart ESP32 if OTA mode changes
  {
    Serial.println("Restarting ESP32...");
    ESP.restart();
  }

  if (otaEnabled) // Conditionally handle OTA server
  {
      server.handleClient();
      yield();
  }
  else
  {
    if (digitalRead(enableRealGPS) == HIGH)
    {
        if (SerialBT.available()) { // Check if GPS data is available
          String gpsData = "";
          while (SerialBT.available()) {
            char c = SerialBT.read(); // Read a character from GPS
            gpsData += c;             // Append to string
          }
          Serial.println("GPS Data: " + gpsData); // Print GPS data
        }
        delay(50);
    }
    else
    {
      boat.update();
      unsigned long currentMillis = millis();
      if (currentMillis - lastNMEATime >= 1000) // Send NMEA data every 200ms
      {
        lastNMEATime = currentMillis;
        boat.print();
      }
    }
  }
}
