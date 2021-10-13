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

    clock.begin();
    clock.fillByYMD(2021, 10, 22);
    clock.fillByHMS(10, 0, 0);
    clock.fillDayOfWeek(FRI);
    clock.setTime();

    Serial.begin(9600);

    if (isRedButtonPressed()) launchMode(configuration);
    else launchMode(standard);
}

void loop() {
    String readings;
    switch (currentMode) {
        case standard:
        case economique:
            if (isElapsed(timer, getConfig(LOG_INTERVAL))) {
                // horodatage
                readings = String(getDateTime()) + ": ";

                // pression
                if (getConfig(PRESSURE)) readings += String(getPressure()) +  "hPa | ";
                // temperature
                if (getConfig(TEMP_AIR)) readings += String(getTemperature()) + " °C | ";
                // hygrometrie
                if (getConfig(HYGR)) readings += String(getHumidity()) + "% | ";

                // luminosite
                if (getConfig(LUMIN)) readings += getLightSensorValue();

                if (currentMode == economique) {
                    if (gpsToggle) readings += getGpsData() + " |";
                    gpsToggle = !gpsToggle;
                } else readings += getGpsData() + " |";
                // GPS
            }
        case maintenance:
        case configuration:
            if (isElapsed(timer, 30*60)) launchMode(standard);
            else configCmdHandler();
        default:
            break;
    }
}
