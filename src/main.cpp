#include <Arduino.h>
#include "WiFiManager.h"
#include "OTAUpdateServer.h"
#include <WebServer.h>
#include <Secrets.h>
#include "WebServer.h"

// OTA server configuration
const char* host = "esp32-raw-gps";

// Define GPIO pin to monitor
const int enableOtaPin = 25; // Replace with the actual pin you are using

// Create objects
WiFiManager wifiManager(wifi_setup, sizeOfWifiSetup);;
WebServer server(80); // Shared WebServer object
OTAUpdateServer otaUpdateServer(server);
bool otaEnabled = false; // Track OTA state

void setup() {
    Serial.begin(115200);

    // Setup GPIO pin mode
    pinMode(enableOtaPin, INPUT);

    // Setup WiFi
    wifiManager.setup(host);

    // Setup OTA server
    otaUpdateServer.setup();

    // Start the shared server
    server.begin();
    Serial.println("Web server started");

    // Initialize Serial and Serial1 at 9600 baud
    Serial1.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
    int pinState = digitalRead(enableOtaPin);

    if (pinState == HIGH) {
        if (!otaEnabled) {
            Serial.println("Enabling OTA mode...");
            otaEnabled = true;
        }
    } else {
        if (otaEnabled) {
            Serial.println("Disabling OTA mode...");
            otaEnabled = false;
        }
    }

    // Conditionally handle OTA server
    if (otaEnabled) {
        server.handleClient();
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
