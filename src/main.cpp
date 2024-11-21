#include <Arduino.h>
#include "WiFiManager.h"
#include "OTAUpdateServer.h"
#include <ArduinoOTA.h>
#include "OTA.h"
#include <WebServer.h>
#include <Secrets.h>
#include "WebServer.h"



// OTA server configuration
const char *host = "esp32-raw-gps";

// Define GPIO pin to monitor
const int enableOtaPin = 25; // Replace with the actual pin you are using

unsigned long lastNMEATime = 0;

// Create objects
WiFiManager wifiManager(wifi_setup, sizeOfWifiSetup);
;
WebServer server(80); // Shared WebServer object
OTAUpdateServer otaUpdateServer(server);
bool otaEnabled = false; // Track OTA state

void setup()
{
  Serial.begin(115200);

  // Setup GPIO pin mode
  pinMode(enableOtaPin, INPUT);

  if (digitalRead(enableOtaPin) == HIGH)
  {
    Serial.println("Enabling OTA mode...");
    // Setup WiFi
    wifiManager.setup(host);

    // Setup OTA server
    otaUpdateServer.setup();
    otaSetup();

    // Start the shared server
    server.begin();
    otaEnabled = true;
    Serial.println("Enabled OTA mode");
  }
  else
  {
    Serial.println("Enabling NMEA mode...");
    Serial1.begin(9600, SERIAL_8N1, 16, 17);
  }
}

void loop()
{
  if ((!otaEnabled && digitalRead(enableOtaPin) == HIGH) || (otaEnabled && digitalRead(enableOtaPin) == LOW)) // Restart ESP32 if OTA mode changes
  {
    Serial.println("Restarting ESP32...");
    ESP.restart();
  }

  unsigned long currentMillis = millis();
  if (otaEnabled) // Conditionally handle OTA server
  {
    server.handleClient();
    ArduinoOTA.handle();
  }
  else if (currentMillis - lastNMEATime >= 200) // Send NMEA data every 200ms
  {
    lastNMEATime = currentMillis;
    Serial1.println("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");
  }
  else // Delay to prevent high CPU usage
  {
    delay(10);
  }
}

// #include <Arduino.h>

// void setup()
// {
//     // Initialize Serial and Serial1 at 9600 baud
//     Serial.begin(9600);
//     Serial1.begin(9600, SERIAL_8N1, 16, 17);
// }

// void loop()
// {
//     // Do nothing in the loop
//     Serial.println("Hello World");
//     Serial1.println("Hello World");
//     delay(1000);
// }
