#include "utils.h"
#include "pins.h"
#include "colors.h"
#include "modes.h"

void setup() {
    // setup buttons
    pinMode(PIN_BUTTON_RED, INPUT);
    pinMode(PIN_BUTTON_GREEN, INPUT);

    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_RED), interruptRed, RISING);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_GREEN), interruptGreen, RISING);

    // setup led and turn off
    led.init();
    setLEDColor(BLACK);

    if (!card.init(SPI_FULL_SPEED, PIN_SD)) {
        Serial.println("Can't detect SD Card!");
        launchErrorSequence(sdAccessDenied, true);
    }

    // Initializing volume
    if (!volume.init(&card)) {
        Serial.println("Error opening volume.");
        launchErrorSequence(sdAccessDenied, true);
    }

    // setup bme (temp, pressure, humidity)
    if (bme.begin(ADDR_BME280)) {
        Serial.println("BME280 initialization error!");
        launchErrorSequence(probeFetchError, true);
    }

    serialGPS.begin(9600);

    Serial.begin(9600);

    if (isRedButtonPressed()) launchMode(configuration);
    else launchMode(standard);
}

void loop() {
    
}
