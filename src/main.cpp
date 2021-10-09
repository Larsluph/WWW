#include "utils.h"
#include "pins.h"
#include "colors.h"
#include "modes.h"

void setup() {
    // setup buttons
    pinMode(PIN_BUTTON_RED, INPUT);
    pinMode(PIN_BUTTON_GREEN, INPUT);

    // setup led and turn off
    led.init();
    setLEDColor(BLACK);

    // setup bme (temp, pressure, humidity)
    if (bme.begin(ADDR_BME280)) {
        launchErrorSequence(probeFetchError, true);
    }

    serialGPS.begin(9600);

    Serial.begin(9600);
}

void loop() {
    if (isElapsed(lastIter, getConfig(LOG_INTERVAL)*60000)) {
        launchMode((Mode)currentMode);
    }
}
