#include <Arduino.h>
#include <math.h>

String calculateChecksum(String sentence);
void outputNMEA();

void outputNMEA() {
    String gga = "$GPGGA,123519,3746.4940,N,12225.1640,W,1,08,0.9,545.4,M,46.9,M,,*";
    String gga = "$GPGGA,000116,1582.8610,N,03742.6187,W,1,08,0.9,545.4,M,46.9,M,,*"
    gga += calculateChecksum(gga);
    Serial1.println(gga);

    String vtg = "$GPVTG,45.00,T,,M,5.00,N,,K*";
    vtg += calculateChecksum(vtg);
    Serial1.println(vtg);

    // Debugging output
    Serial.println("Serial1: " + gga);
    Serial.println("Serial1: " + vtg);
}

String calculateChecksum(String sentence) {
    // Ensure the sentence starts with '$' and contains '*'
    int startIdx = sentence.indexOf('$');
    int endIdx = sentence.indexOf('*');
    if (startIdx == -1 || endIdx == -1 || endIdx <= startIdx) {
        // Invalid sentence format
        return "";
    }

    byte checksum = 0;
    // XOR all characters between '$' and '*'
    for (int i = startIdx + 1; i < endIdx; i++) {
        checksum ^= sentence[i];
    }

    // Convert checksum to a two-character uppercase hexadecimal string
    char checksumStr[3];
    sprintf(checksumStr, "%02X", checksum);

    return String(checksumStr);
}
