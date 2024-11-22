#include "WiFi.h"
#include "WebServer.h"
#include "Update.h"
const char *ssid = "TempESP32";
const char *pass = "hejhejhej";
WebServer server(80);
String serverIndex =
    "<style>"
    "body { font-family: Arial, sans-serif; margin: 20px; }"
    "form { background-color: #f9f9f9; padding: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }"
    "input[type='file'] { display: block; margin-bottom: 15px; padding: 5px; border: 1px solid #ccc; border-radius: 3px; }"
    "input[type='submit'] { background-color: #4CAF50; color: white; border: none; padding: 10px 20px; border-radius: 3px; cursor: pointer; }"
    "input[type='submit']:hover { background-color: #45a049; }"
    "</style>"
    "<form method='POST' action='/update' enctype='multipart/form-data'>"
    "<h2>Update Firmware</h2>"
    "<label for='update'>Select a file to upload:</label>"
    "<input type='file' name='update' id='update'><br>"
    "<input type='submit' value='Upload'>"
    "</form>";

unsigned long t_start, t_stop;
void setup(void)
{
    Serial.begin(115200);
    Serial.println();
    Serial.println(ESP.getSdkVersion());
    WiFi.mode(WIFI_STA);
    WiFi.begin("Connect", "hejhejhej");
    Serial.println("Connecting to WiFi");
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(100);
        ESP.restart();
    }
    Serial.println("Connected successfully!");
    Serial.println("Local IP: " + WiFi.localIP().toString());

    server.on("/serverIndex", HTTP_GET, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex); });
    server.on("/update", HTTP_POST, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(500);  // else server not be in time send "FAIL" or "OK"
    ESP.restart(); }, []()
              {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      t_start = millis();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        t_stop = millis();
        Serial.print("Time UPLOAD: "); Serial.print((t_stop - t_start) / 1000); Serial.println(" sec.");
        Serial.print("Speed UPLOAD: "); Serial.print((float(upload.totalSize / 1024)*8) / float((t_stop - t_start) / 1000)); Serial.println(" Kbit/s");
        Serial.printf("Update Success: %u bytes\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    } });
    server.begin();
}
void loop(void)
{
    server.handleClient();
}