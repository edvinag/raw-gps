#ifndef OTA_UPDATE_SERVER_SECURE_H
#define OTA_UPDATE_SERVER_SECURE_H

#include "WebServerSecure.h"
#include <Update.h>

class OTAUpdateServerSecure {
public:
    OTAUpdateServerSecure(WebServerSecure& sharedServer) : server(sharedServer) {}

    void setup() {
        // Register the root page for OTA
        server.on("/", "GET", handleRoot);

        // Register the update endpoint
        server.on("/update", "POST", handleUpload);

        Serial.println("OTA update server routes registered.");
    }

private:
    WebServerSecure& server;

    static void handleRoot(HTTPRequest* req, HTTPResponse* res) {
        res->setHeader("Connection", "close");
        res->setHeader("Content-Type", "text/html");
        res->println(
            "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
            "<form method='POST' action='/update' enctype='multipart/form-data' id='upload_form'>"
            "<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
            "<label id='file-input' for='file'>   Choose file...</label>"
            "<input type='submit' class=btn value='Update'>"
            "<br><br>"
            "<div id='prg'></div>"
            "<br><div id='prgbar'><div id='bar'></div></div><br></form>"
            "<script>"
            "function sub(obj){"
            "var fileName = obj.value.split('\\\\');"
            "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
            "};"
            "$('form').submit(function(e){"
            "e.preventDefault();"
            "var form = $('#upload_form')[0];"
            "var data = new FormData(form);"
            "$.ajax({"
            "url: '/update',"
            "type: 'POST',"
            "data: data,"
            "contentType: false,"
            "processData:false,"
            "xhr: function() {"
            "var xhr = new window.XMLHttpRequest();"
            "xhr.upload.addEventListener('progress', function(evt) {"
            "if (evt.lengthComputable) {"
            "var per = evt.loaded / evt.total;"
            "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
            "$('#bar').css('width',Math.round(per*100) + '%');"
            "}"
            "}, false);"
            "return xhr;"
            "},"
            "success:function(d, s) {"
            "console.log('success!') "
            "},"
            "error: function (a, b, c) {"
            "}"
            "});"
            "});"
            "</script>"
            "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
            "input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
            "#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
            "#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
            "form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
            ".btn{background:#3498db;color:#fff;cursor:pointer}</style>"
        );
    }

    static void handleUpload(HTTPRequest* req, HTTPResponse* res) {
        if (req->getMethod() != "POST") {
            res->setStatusCode(405);
            res->setStatusText("Method Not Allowed");
            res->println("Only POST requests are allowed.");
            return;
        }

        size_t totalSize = 0;
        bool success = true;

        // Read the firmware update data
        while (!req->requestComplete()) {
            uint8_t buffer[1024];
            size_t length = req->readBytes(buffer, sizeof(buffer));

            if (totalSize == 0) {
                Serial.println("Starting OTA update...");
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                    Update.printError(Serial);
                    success = false;
                    break;
                }
            }

            if (Update.write(buffer, length) != length) {
                Update.printError(Serial);
                success = false;
                break;
            }

            totalSize += length;
        }

        if (success && Update.end(true)) {
            Serial.printf("Update Success: %u bytes\nRebooting...\n", totalSize);
            res->setHeader("Content-Type", "text/plain");
            res->println("Update Success!");
            ESP.restart();
        } else {
            Update.printError(Serial);
            res->setHeader("Content-Type", "text/plain");
            res->println("Update Failed!");
        }
    }
};

#endif
