#ifndef BOAT_SIMULATOR_H
#define BOAT_SIMULATOR_H

#include <Arduino.h>
#include <math.h>

// Pin definitions
const int rudderRightPin = 15;
const int rudderLeftPin = 16;

// Boat state variables
extern float latitude;       // Initial latitude (e.g., San Francisco)
extern float longitude;      // Initial longitude (e.g., San Francisco)
extern float heading;        // Heading in radians (yaw)
extern float angularVelocity; // Change in heading per second

// Dynamic constants
const float constantSpeed = 4.11556;  // Constant speed in m/s (8 knots)
const float maxTurnRate = 0.1;        // Max turning rate in radians per second
const float inertia = 0.95;           // Rate at which turning adjusts to rudder

// Earth constants
const float metersPerDegreeLat = 111320.0; // Approximate meters per degree latitude
extern float metersPerDegreeLon;           // Calculated based on latitude

// Update interval
extern unsigned long lastUpdateTime;
const unsigned long updateInterval = 50; // 50 ms

// Function declarations
void setupBoatSimulator();
void updateBoatSimulator();
void outputNMEA();

#endif // BOAT_SIMULATOR_H
