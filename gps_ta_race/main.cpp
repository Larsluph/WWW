#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial serialGPS(5, 6);

void setup() {
    Serial.begin(9600);
    while (!Serial);
    serialGPS.begin(9600);
}

void loop() {
    if (serialGPS.available())
    {
        // Serial.print((char) serialGPS.read());
        String data = serialGPS.readStringUntil('\n');
        if (data.lastIndexOf("$GPGGA") != -1) {
            Serial.print("//");Serial.print(data.substring(data.lastIndexOf("$GPGGA"), data.length()-1));Serial.println("//");
        }
    } else {
        // Serial.println("no data.");
    }
    // delay(2000);
}
