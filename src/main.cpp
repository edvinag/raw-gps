#include <Arduino.h>
#include "WiFiManager.h"
#include "OTAUpdateServer.h"
// #include <ArduinoOTA.h>
// #include "OTAHandler.h"
#include <WebServer.h>
#include <Credentials.h>
#include "BoatSimulator.h"
#include "GpsCheck.h"

// OTA server configuration
const char *host = "esp32-raw-gps";

// Define GPIO pin to monitor
const int enableOtaPin = 25; // Replace with the actual pin you are using

unsigned long lastNMEATime = 0;

// Create objects
WiFiManager wifiManager(wifiCredentials, sizeOfWifiCredentials);
;
WebServer server(80); // Shared WebServer object
OTAUpdateServer otaUpdateServer(server);
// OTAHandler otaHandler;
GpsCheck gpsCheck;
BoatSimulator boat(37.7749, -122.4194); // San Francisco coordinates

bool otaEnabled = false; // Track OTA state

void setup()
{
  delay(1000);
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); // Turn the LED on

  // // Setup GPIO pin mode
  pinMode(enableOtaPin, INPUT);

  if (digitalRead(enableOtaPin) == HIGH)
  {
    Serial.println("Enabling OTA mode...");
    // Setup WiFi
    wifiManager.setup(host);
    otaUpdateServer.setup();
    // otaHandler.setup();

    // Start the shared server
    server.begin();
    otaEnabled = true;
    Serial.println("Enabled OTA mode");
  }
  else
  {
    Serial.println("Enabling NMEA mode...");
    // Serial1.begin(9600, SERIAL_8N1, 16, 17);
    boat.setup();
    // Serial2.begin(9600, SERIAL_8N1, 12, 13);
  }
}

void loop()
{
  digitalWrite(2, LOW); // Turn the LED on

    //   server.handleClient();
    // yield();
    // delay(20);
  if (otaUpdateServer.isOTAInProgress())
  {
    server.handleClient();
    yield();
  }
  else
  {

    if (digitalRead(enableOtaPin) == !otaEnabled) // Restart ESP32 if OTA mode changes
    {
      Serial.println("Restarting ESP32...");
      ESP.restart();
    }

    unsigned long currentMillis = millis();
    if (otaEnabled) // Conditionally handle OTA server
    {
       server.handleClient();
       yield();
  //     // if (!otaHandler.isRunning())
  //     // {
  //     //   server.handleClient();
  //     // }
  //     // otaHandler.handle();
  //     // ArduinoOTA.handle();
    }
    else
    {
      gpsCheck.handle();
      boat.update();
      if (currentMillis - lastNMEATime >= 1000) // Send NMEA data every 200ms
      {
        lastNMEATime = currentMillis;
        // Serial.println("Serial1: Sending NMEA data");
        // outputNMEA();
        gpsCheck.print();
        boat.print();
      }
    }
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
