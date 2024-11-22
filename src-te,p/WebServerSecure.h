#ifndef WEB_SERVER_SECURE_H
#define WEB_SERVER_SECURE_H

#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

// Include your certificate and key files here
#include "https/cert.h"
#include "https/private_key.h"

using namespace httpsserver;

class WebServerSecure {
public:
    WebServerSecure() 
      : cert(example_crt_DER, example_crt_DER_len, example_key_DER, example_key_DER_len),
        server(&cert) {}

    void begin() {
        Serial.println("Starting HTTPS server...");
        server.start();
        if (server.isRunning()) {
            Serial.println("HTTPS server is running.");
        } else {
            Serial.println("Failed to start HTTPS server.");
        }
    }

    void on(const String& path, const String& method, void (*handler)(HTTPRequest*, HTTPResponse*)) {
        ResourceNode *node = new ResourceNode(path.c_str(), method.c_str(), handler);
        server.registerNode(node);
    }

    void setDefaultHandler(void (*handler)(HTTPRequest*, HTTPResponse*)) {
        ResourceNode *node = new ResourceNode("", "ANY", handler);
        server.setDefaultNode(node);
    }

    void loop() {
        server.loop();
    }

private:
    SSLCert cert;
    HTTPSServer server;
};

#endif
