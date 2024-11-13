#include "BoatSimulator.h"

// Define global variables
float latitude = 37.7749;      // Initial latitude (e.g., San Francisco)
float longitude = -122.4194;   // Initial longitude (e.g., San Francisco)
float heading = 0.0;           // Heading in radians (yaw)
float angularVelocity = 0.0;   // Change in heading per second
float metersPerDegreeLon;      // Calculated based on latitude
unsigned long lastUpdateTime = 0;

void setupBoatSimulator() {
    // Initialize Serial1 for output
    Serial1.begin(4800);

    // Configure control pins
    pinMode(rudderRightPin, INPUT_PULLUP);
    pinMode(rudderLeftPin, INPUT_PULLUP);

    // Start serial for debugging
    Serial.begin(115200);
}

void updateBoatSimulator() {
    unsigned long currentTime = millis();

    // Read rudder inputs to adjust heading
    if (digitalRead(rudderRightPin) == LOW) {
        angularVelocity += maxTurnRate * (1 - inertia);  // Gradual right turn
    } else if (digitalRead(rudderLeftPin) == LOW) {
        angularVelocity -= maxTurnRate * (1 - inertia);  // Gradual left turn
    }

    // Apply inertia to angular velocity
    angularVelocity *= inertia;

    // Update heading with angular velocity
    heading += angularVelocity * updateInterval / 1000.0;
    if (heading < 0) heading += 2 * PI;
    if (heading >= 2 * PI) heading -= 2 * PI;

    // Calculate meters per degree longitude based on current latitude
    metersPerDegreeLon = metersPerDegreeLat * cos(latitude * PI / 180.0);

    // Update latitude and longitude based on constant speed and heading
    float deltaLat = (constantSpeed * cos(heading) * updateInterval / 1000.0) / metersPerDegreeLat;
    float deltaLon = (constantSpeed * sin(heading) * updateInterval / 1000.0) / metersPerDegreeLon;
    latitude += deltaLat;
    longitude += deltaLon;

    // Output NMEA data at set intervals
    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        outputNMEA();
    }
}

void outputNMEA() {
    float headingDegrees = heading * 180.0 / PI;
    float speedKnots = constantSpeed * 1.94384;  // Constant speed in knots (8 knots)

    // GGA sentence (Position fix data)
    String gga = "$GPGGA,123519," + String(latitude, 5) + ",N," + String(longitude, 5) + ",W,1,08,0.9,545.4,M,46.9,M,,*47";
    Serial1.println(gga);

    // VTG sentence (Course over ground and speed)
    String vtg = "$GPVTG," + String(headingDegrees, 1) + ",T,,M," + String(speedKnots, 1) + ",N,,K*68";
    Serial1.println(vtg);

    // Debugging output
    Serial.println(gga);
    Serial.println(vtg);
}
