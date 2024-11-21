#include <TinyGPS++.h>
TinyGPSPlus tinyGPS;

class GpsCheck
{
public:
    GpsCheck() : tinyGPS() {
        Serial2.begin(9600, SERIAL_8N1, 12, 13);
    }

    void handle()
    {
        if (Serial2.available())
        {
            while (Serial2.available())
            {
                tinyGPS.encode(Serial2.read());
            }
        }
    }

    void print()
    {
        if (tinyGPS.location.isValid())
        {
            Serial.println("");
            Serial.println("Location is valid");
            Serial.print("Latitude: ");
            Serial.println(tinyGPS.location.lat(), 6);
            Serial.print("Longitude: ");
            Serial.println(tinyGPS.location.lng(), 6);

        }
        else
        {
            Serial.println("Location is not valid");
        }
    }

private:
    TinyGPSPlus tinyGPS;
};