#include <Arduino.h>
#include "BoatSimulator.h"
#include "BLEconnect.h"

#define ENABLEREALGPS 26
#define REDLED 2

unsigned long lastNMEATime = 0;

BoatSimulator boat(57.57367, 11.92692); // Gothenburg, Sweden

void setup()
{
  Serial.begin(115200);
  
  pinMode(REDLED, OUTPUT);
  pinMode(ENABLEREALGPS, INPUT);

  digitalWrite(REDLED, HIGH);

  Serial.println("Enabling NMEA mode...");
  boat.setup();
  if (digitalRead(ENABLEREALGPS) == HIGH)
  {
    BLEsetup();
  }
}

void loop()
{
  digitalWrite(REDLED, LOW);

  if (digitalRead(ENABLEREALGPS) == HIGH)
  {
    if(!SerialBT.isReady())
    {
      Serial.println("BLE not ready, setting up...");
      BLEsetup();
    }
    else
    {
      BLEloop();
      delay(200);
    }
  }
  else
  {
    boat.update();
    unsigned long currentMillis = millis();
    if (currentMillis - lastNMEATime >= 1000) // Print NMEA sentence every second to Serial Monitor 
    {
      lastNMEATime = currentMillis;
      boat.print();
    }
  }
}
