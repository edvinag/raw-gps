#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <ESPmDNS.h>

class WiFiManager {
public:
    void setup(const char* ssid, const char* password, const char* host) {
        WiFi.begin(ssid, password);
        Serial.println("");

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        // mDNS setup
        if (!MDNS.begin(host)) {
            Serial.println("Error setting up MDNS responder!");
            while (1) {
                delay(1000);
            }
        }
        Serial.println("mDNS responder started");
    }

    IPAddress getLocalIP() {
        return WiFi.localIP();
    }
};

#endif
