#ifndef BOAT_SIMULATOR_H
#define BOAT_SIMULATOR_H

#include <Arduino.h>
#include <math.h>

class BoatSimulator {
private:
    // Pin definitions
    const int rudderRightPin = 18;
    const int rudderLeftPin = 19;

    // Dynamic constants
    const float constantSpeed = 4.11556;  // Constant speed in m/s (8 knots)
    const float maxTurnRate = 0.1;        // Max turning rate in radians per second
    const float inertia = 0.95;           // Rate at which turning adjusts to rudder

    // Earth constants
    const float metersPerDegreeLat = 111320.0; // Approximate meters per degree latitude
    float metersPerDegreeLon;

    // Boat state variables
    float latitude;
    float longitude;
    float heading;
    float angularVelocity;

    // Position in meters from the starting point
    float xPosition;
    float yPosition;

    // Update interval
    unsigned long lastUpdateTime;
    const unsigned long updateInterval = 50; // 50 ms

    // Helper functions
    void convertToLatLon();
    String calculateChecksum(String sentence);
    String getTimestamp();
    String convertToNMEA(double decimalCoord, String type);

public:
    // Constructor
    BoatSimulator(float initialLat, float initialLon);

    // Public methods
    void setup();
    void update();
    void outputNMEA();
    void print();
};

// Implementation

BoatSimulator::BoatSimulator(float initialLat, float initialLon)
    : latitude(initialLat), longitude(initialLon), heading(PI / 4.0), angularVelocity(0.0),
      xPosition(0.0), yPosition(0.0), lastUpdateTime(0) {}

void BoatSimulator::setup() {
    lastUpdateTime = millis();
    Serial1.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(rudderRightPin, INPUT_PULLUP);
    pinMode(rudderLeftPin, INPUT_PULLUP);
    Serial.begin(115200);
}

void BoatSimulator::update() {
    unsigned long currentTime = millis();

    if (digitalRead(rudderRightPin) == LOW) {
        angularVelocity += maxTurnRate * (1 - inertia);
    } else if (digitalRead(rudderLeftPin) == LOW) {
        angularVelocity -= maxTurnRate * (1 - inertia);
    }

    angularVelocity *= inertia;

    heading += angularVelocity * updateInterval / 1000.0;
    if (heading < 0) heading += 2 * PI;
    if (heading >= 2 * PI) heading -= 2 * PI;

    metersPerDegreeLon = metersPerDegreeLat * cos(latitude * PI / 180.0);

    float deltaX = constantSpeed * sin(heading) * updateInterval / 1000.0;
    float deltaY = constantSpeed * cos(heading) * updateInterval / 1000.0;
    xPosition += deltaX;
    yPosition += deltaY;

    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;
        convertToLatLon();
        outputNMEA();
    }
}

void BoatSimulator::convertToLatLon() {
    latitude += yPosition / metersPerDegreeLat;
    longitude += xPosition / metersPerDegreeLon;
}

String BoatSimulator::convertToNMEA(double decimalCoord, String type) {
    char hemisphere = (type == "lat") ? ((decimalCoord >= 0) ? 'N' : 'S') : ((decimalCoord >= 0) ? 'E' : 'W');
    decimalCoord = abs(decimalCoord);
    int degrees = (type == "lat") ? int(decimalCoord) : int(decimalCoord);
    double minutes = (decimalCoord - degrees) * 60;

    char nmeaCoord[20];
    sprintf(nmeaCoord, (type == "lat") ? "%02d%07.4f,%c" : "%03d%07.4f,%c", degrees, minutes, hemisphere);
    return String(nmeaCoord);
}

void BoatSimulator::outputNMEA() {
    float headingDegrees = heading * 180.0 / PI;
    float speedKnots = constantSpeed * 1.94384;
    String timestamp = getTimestamp();

    String nmeaLatitude = convertToNMEA(latitude, "lat");
    String nmeaLongitude = convertToNMEA(longitude, "lon");
    
    // Construct GPRMC sentence
    String rmc = "$GPRMC," + timestamp + ",A," + nmeaLatitude + "," + nmeaLongitude + "," +
                 String(speedKnots, 1) + "," + String(headingDegrees, 2) + ",230924,,,A*";

    rmc += calculateChecksum(rmc); // Append checksum
    Serial1.print(rmc + "\r\n");   // Send GPRMC sentence
    // Serial.println(rmc);           // Optionally log to Serial

    //Construct a GGA sentence
    String gga = "$GPGGA," + timestamp + "," + nmeaLatitude + "," + nmeaLongitude + ",1,08,0.9,545.4,M,46.9,M,,*";
    gga += calculateChecksum(gga);
    Serial1.print(gga + "\r\n");

    // //Construct a VTG sentence
    // String vtg = "$GPVTG," + String(headingDegrees, 2) + ",T,,M," + String(speedKnots, 1) + ",N,,K*";
    // vtg += calculateChecksum(vtg);
    // Serial1.print(vtg + "\r\n");
    // Serial.println(vtg);
}

String BoatSimulator::getTimestamp() {
    unsigned long currentMillis = millis();
    unsigned long totalSeconds = currentMillis / 1000;
    unsigned long hours = (totalSeconds / 3600) % 24;
    unsigned long minutes = (totalSeconds / 60) % 60;
    unsigned long seconds = totalSeconds % 60;

    char timestamp[7];
    sprintf(timestamp, "%02lu%02lu%02lu", hours, minutes, seconds);
    return String(timestamp);
}

String BoatSimulator::calculateChecksum(String sentence) {
    int startIdx = sentence.indexOf('$');
    int endIdx = sentence.indexOf('*');
    if (startIdx == -1 || endIdx == -1 || endIdx <= startIdx){
        Serial.println("Invalid sentence format of: " + sentence);
        return "";
    }

    byte checksum = 0;
    for (int i = startIdx + 1; i < endIdx; i++) checksum ^= sentence[i];

    char checksumStr[3];
    sprintf(checksumStr, "%02X", checksum);
    return String(checksumStr);
}

void BoatSimulator::print() {
    String output = "";
    output += "Latitude: " + String(latitude, 6) + "\n";
    output += "Longitude: " + String(longitude, 6) + "\n";
    output += "Heading: " + String(heading, 6) + "\n";
    output += "Angular Velocity: " + String(angularVelocity, 6) + "\n";
    output += "X Position: " + String(xPosition, 6) + "\n";
    output += "Y Position: " + String(yPosition, 6) + "\n";
    Serial.println(output);
}

#endif // BOAT_SIMULATOR_H
