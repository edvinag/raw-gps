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

// Enter the name your GPS reports
String MyGPS = "Name: Garmin GLO 2 #38ad1";

#define BT_DISCOVER_TIME 10000
esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to request pincode confirmation
esp_spp_role_t role = ESP_SPP_ROLE_SLAVE;  // or ESP_SPP_ROLE_MASTER

// std::map<BTAddress, BTAdvertisedDeviceSet> btDeviceList;

void BLEsetup()
{
  Serial.begin(115200);
  if (!SerialBT.begin("ESP32test", true))
  {
    Serial.println("========== serialBT failed!");
    abort();
  }

  Serial.println("Starting discoverAsync...");
  BTScanResults *btDeviceList = SerialBT.getScanResults(); // maybe accessing from different threads!
  if (SerialBT.discoverAsync([](BTAdvertisedDevice *pDevice)
                             {
      // BTAdvertisedDeviceSet*set = reinterpret_cast<BTAdvertisedDeviceSet*>(pDevice);
      // btDeviceList[pDevice->getAddress()] = * set;
      Serial.printf(">>>>>>>>>>>Found a new device asynchronously: %s\n", pDevice->toString().c_str());
      //Test to see that correct device is found, else reset and try again
      String teststring= pDevice->toString().c_str();
      if(!teststring.startsWith(MyGPS))ESP.restart(); }))
  {
    delay(BT_DISCOVER_TIME);
    Serial.print("Stopping discoverAsync... ");
    SerialBT.discoverAsyncStop();
    Serial.println("discoverAsync stopped");
    delay(1000);
    if (btDeviceList->getCount() > 0)
    {
      BTAddress addr;
      int channel = 0;
      Serial.println("Found devices:");
      for (int i = 0; i < btDeviceList->getCount(); i++)
      {
        BTAdvertisedDevice *device = btDeviceList->getDevice(i);
        Serial.printf(" ----- %s  %s %d\n", device->getAddress().toString().c_str(), device->getName().c_str(), device->getRSSI());
        std::map<int, std::string> channels = SerialBT.getChannels(device->getAddress());
        Serial.printf("scanned for services, found %d\n", channels.size());
        for (auto const &entry : channels)
        {
          Serial.printf("     channel %d (%s)\n", entry.first, entry.second.c_str());
        }
        if (channels.size() > 0)
        {
          addr = device->getAddress();
          channel = channels.begin()->first;
        }
      }
      if (addr)
      {
        Serial.printf("connecting to %s - %d\n", addr.toString().c_str(), channel);

        SerialBT.connect(addr, channel, sec_mask, role);
      }
    }
    else
    {
      Serial.println("Didn't find any devices");
      ESP.restart();
    }
  }
  else
  {
    Serial.println("Error on discoverAsync f.e. not workin after a \"connect\"");
    ESP.restart();
  }
}

void BLEloop()
{
  if (!SerialBT.isClosed() && SerialBT.connected())
  {
    if (SerialBT.available())
    {
      while (SerialBT.available())
      {
        int c = SerialBT.read();
        if (c >= 0)
        {
          Serial.print((char)c);
          Serial1.print((char)c);
        }
      }
    }
  }
}
