/**
 * Bluetooth Classic Example
 * Scan for devices - asyncronously, print device as soon as found
 * query devices for SPP - SDP profile
 * connect to first device offering a SPP connection
 *
 * Example python server:
 * source: https://gist.github.com/ukBaz/217875c83c2535d22a16ba38fc8f2a91
 *
 * Tested with Raspberry Pi onboard Wifi/BT, USB BT 4.0 dongles, USB BT 1.1 dongles,
 * 202202: does NOT work with USB BT 2.0 dongles when esp32 aduino lib is compiled with SSP support!
 *         see https://github.com/espressif/esp-idf/issues/8394
 *
 * use ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE in connect() if remote side requests 'RequireAuthentication': dbus.Boolean(True),
 * use ESP_SPP_SEC_NONE or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE in connect() if remote side has Authentication: False
 */

#include <map>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role = ESP_SPP_ROLE_SLAVE;  // or ESP_SPP_ROLE_MASTER
BTAddress addr("14:13:0b:e3:8a:d1");
int channel = 1;

String latestRMC = "";
String latestGGA = "";
unsigned long lastSentTime = 0;         // To track when to send the messages
const unsigned long sendInterval = 250; // Interval in milliseconds

void BLEsetup()
{
  if (!SerialBT.begin("ESP32-Raw-GPS", true))
  {
    Serial.println("Failed to initialize SerialBT");
    abort();
  }

  Serial.println("Connecting to Garmin GLO 2 #38ad1");
  SerialBT.connect(addr, channel, sec_mask, role);

  if (SerialBT.connected())
  {
    Serial.println("Connected to Garmin GLO 2 #38ad1");
  }
  else
  {
    Serial.println("Failed to connect to Garmin GLO 2 #38ad1, restarting...");
    ESP.restart();
  }
}

void BLEloop()
{
  static String currentLine = ""; // Buffer for the current NMEA line

  if (!SerialBT.isClosed() && SerialBT.connected())
  {
    while (SerialBT.available())
    {
      char c = (char)SerialBT.read();
      Serial.print(c); // Print the received character to the Serial Monitor

      if (c == '\n' || c == '\r')
      { // End of an NMEA line
        if (currentLine.startsWith("$GPRMC"))
        {
          latestRMC = currentLine; // Save the latest RMC message
        }
        else if (currentLine.startsWith("$GPGGA"))
        {
          latestGGA = currentLine; // Save the latest GGA message
        }
        currentLine = ""; // Clear the buffer for the next line
      }
      else
      {
        currentLine += c; // Append character to the current line
      }
    }
  }

  // Check if it's time to send the latest messages
  unsigned long currentTime = millis();
  if (currentTime - lastSentTime >= sendInterval)
  {
    if (!latestRMC.isEmpty())
    {
      Serial1.println(latestRMC); // Send the latest RMC message
    }
    if (!latestGGA.isEmpty())
    {
      Serial1.println(latestGGA); // Send the latest GGA message
    }
    lastSentTime = currentTime; // Update the last sent time
  }
}
