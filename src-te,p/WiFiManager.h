#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <ESPmDNS.h>

// Structure to hold network configuration
struct WifiCredentials {
    const char* ssid;
    const char* password;
    IPAddress local_IP;
    IPAddress gateway;
    IPAddress subnet;
};

class WiFiManager {
public:
    WiFiManager(WifiCredentials* setups, int size) : wifiCredentials(setups), setupCount(size) {}

    void setup(const char* mdnsName) {
        // Serial.begin(115200);
        // WiFi.mode(WIFI_STA);

        // Scan for networks
        // Serial.print("Scan start for WiFi... ");
        // int n = WiFi.scanNetworks();
        // int wifiIndex = 0; // Default to the first configuration
        // Serial.print(n);
        // Serial.println(" network(s) found:");
        // for (int i = 0; i < n; i++) {
        //     Serial.println(WiFi.SSID(i));
        //     for (int j = 0; j < setupCount; j++) {
        //         if (WiFi.SSID(i) == wifiCredentials[j].ssid) {
        //             Serial.println(" - Found SSID");
        //             wifiIndex = j; // Match found
        //         }
        //     }
        // }

        // Configure WiFi
        // if (!WiFi.config(wifiCredentials[wifiIndex].local_IP, wifiCredentials[wifiIndex].gateway, wifiCredentials[wifiIndex].subnet)) {
        //     Serial.println("STA Failed to configure");
        // }
        // delay(1000);
        // Connect to WiFi
        Serial.print("Connecting to ");
        // Serial.println(wifiCredentials[wifiIndex].ssid);
        WiFi.begin("Connect", "hejhejhej");
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Connection Failed! Rebooting...");
            delay(100);
            ESP.restart();
        }
        Serial.println("Connected successfully!");
        Serial.println("Local IP: " + WiFi.localIP().toString());

        // Setup mDNS
        setupMDNS(mdnsName);
    }

    String getLocalIP() {
        return WiFi.localIP().toString();
    }

private:
    WifiCredentials* wifiCredentials;             // Array of network configurations
    int setupCount;                  // Number of networks in the array

    void setupMDNS(const char* mdnsName) {
        if (!MDNS.begin(mdnsName)) {
            Serial.println("Error setting up mDNS responder!");
            while (1) {
                delay(1000); // Wait indefinitely in case of mDNS failure
            }
        }
        Serial.printf("mDNS responder started: http://%s.local\n", mdnsName);
    }
};

#endif
