#include <TinyGPS++.h>

TinyGPSPlus tinyGPS;

class GpsCheck
{
public:
    GpsCheck() : tinyGPS(), lastTime(0), lastHeading(0) {
        Serial2.begin(115200, SERIAL_8N1, 12, 13);
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
            Serial.print("Course: ");
            Serial.println(tinyGPS.course.deg(), 6); // Degrees

            if (tinyGPS.course.isUpdated()) // Use the heading (course) from GPVTG
            {
                double headingRate = getHeadingRate();
                if (headingRate != -1) // -1 indicates insufficient data
                {
                    Serial.print("Heading Rate: ");
                    Serial.println(headingRate, 6); // Radians per second
                }
                else
                {
                    Serial.println("Insufficient data for Heading Rate calculation");
                }
            }
            else
            {
                Serial.println("Heading data not available");
            }
        }
        else
        {
            Serial.println("Location is not valid");
        }
    }

    double getHeadingRate()
    {
        if (tinyGPS.course.isValid() && tinyGPS.time.isValid())
        {
            double currentHeading = tinyGPS.course.deg(); // Heading in degrees
            double currentTime = tinyGPS.time.value() / 1000.0; // Time in seconds

            if (lastTime > 0) // Ensure we have a previous timestamp
            {
                double deltaHeading = currentHeading - lastHeading;

                // Normalize heading difference to range -180 to 180 degrees
                if (deltaHeading > 180)
                    deltaHeading -= 360;
                else if (deltaHeading < -180)
                    deltaHeading += 360;

                // Convert degrees to radians
                deltaHeading = radians(deltaHeading);

                double deltaTime = currentTime - lastTime;
                if (deltaTime > 0)
                {
                    // Update last values and return the calculated yaw rate
                    lastHeading = currentHeading;
                    lastTime = currentTime;
                    return deltaHeading / deltaTime; // Radians per second
                }
            }

            // Update last values if this is the first valid data point
            lastHeading = currentHeading;
            lastTime = currentTime;
        }

        return -1; // Return -1 if there's insufficient data
    }

private:
    TinyGPSPlus tinyGPS;

    double lastTime;
    double lastHeading;
};
