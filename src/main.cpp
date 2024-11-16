#include <Arduino.h>
#include "WiFiManager.h"
#include "OTAUpdateServer.h"
#include <WebServer.h>

// WiFi credentials
const char* ssid = "***";
const char* password = "***";

// OTA server configuration
const char* host = "esp32-raw-gps";

// Create objects
WiFiManager wifiManager;
WebServer server(80); // Shared WebServer object
OTAUpdateServer otaUpdateServer(server);

void setup() {
    Serial.begin(115200);

    // Setup WiFi
    wifiManager.setup(ssid, password, host);

    // Setup OTA server
    otaUpdateServer.setup();

    // Start the shared server
    server.begin();
    Serial.println("Web server started");

    // Initialize Serial and Serial1 at 9600 baud
    Serial1.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
    server.handleClient(); // Handle requests for all components
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
