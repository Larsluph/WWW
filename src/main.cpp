#include "utils.h"
#include "pins.h"
#include "colors.h"
#include "modes.h"

void setup() {
    pinMode(PIN_BUTTON_RED, INPUT);
    pinMode(PIN_BUTTON_GREEN, INPUT);

    led.init();

    serialGPS.begin(9600);

    Serial.begin(9600);
}

void loop() {
    // launchErrorSequence(ErrorCodes::gpsFetchError);
}
