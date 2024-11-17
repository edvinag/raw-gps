#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

// Structure to hold network configuration
struct Network {
    const char* ssid;
    const char* password;
    IPAddress local_IP;
    IPAddress gateway;
    IPAddress subnet;
};

class WiFiManager {
public:
    WiFiManager(Network* setups, int size) : wifiSetups(setups), setupCount(size) {}

    void setup() {
        Serial.begin(115200);
        Serial.println("Booting");
        WiFi.mode(WIFI_STA);

        // Scan for networks
        Serial.print("Scan start ... ");
        int n = WiFi.scanNetworks();
        int wifiIndex = 0; // Default to the first configuration
        Serial.print(n);
        Serial.println(" network(s) found:");
        for (int i = 0; i < n; i++) {
            Serial.println(WiFi.SSID(i));
            for (int j = 0; j < setupCount; j++) {
                if (WiFi.SSID(i) == wifiSetups[j].ssid) {
                    Serial.println(" - Found SSID");
                    wifiIndex = j; // Match found
                }
            }
        }

        // Configure WiFi
        if (!WiFi.config(wifiSetups[wifiIndex].local_IP, wifiSetups[wifiIndex].gateway, wifiSetups[wifiIndex].subnet)) {
            Serial.println("STA Failed to configure");
        }

        // Connect to WiFi
        Serial.print("Connecting to ");
        Serial.println(wifiSetups[wifiIndex].ssid);
        WiFi.begin(wifiSetups[wifiIndex].ssid, wifiSetups[wifiIndex].password);
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        }
        Serial.println("Connected successfully!");
        Serial.println("Local IP: " + WiFi.localIP().toString());
    }

    String getLocalIP() {
        return WiFi.localIP().toString();
    }

private:
    Network* wifiSetups;             // Array of network configurations
    int setupCount;                  // Number of networks in the array
};

#endif
