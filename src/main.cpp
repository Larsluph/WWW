#include "utils.h"
#include "pins.h"
#include "colors.h"
#include "modes.h"

#define DEBUG 0
#define DEBUG_DELAY 1000
#define TEST_BATCH 3

void setup() {
    restoreConfig();

    Serial.begin(9600);
    serialGPS.begin(9600);
    while (!Serial);

    // setup buttons
    pinMode(PIN_BUTTON_RED, INPUT);
    pinMode(PIN_BUTTON_GREEN, INPUT);

    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_RED), interruptRed, RISING);
    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_GREEN), interruptGreen, RISING);

    // setup led and turn off
    led.init();
    setLEDColor(BLACK);

    if (!SD.begin(PIN_SD)) {
        Serial.println("Can't detect SD Card!");
        launchErrorSequence(sdAccessDenied, true);
    }

    // setup bme (temp, pressure, humidity)
    if (!bme.begin(ADDR_BME280)) {
        Serial.println("BME280 initialization error!");
        launchErrorSequence(probeFetchError, true);
    }

    clock.begin();
    clock.fillByYMD(2021, 10, 22);
    clock.fillByHMS(10, 0, 0);
    clock.fillDayOfWeek(FRI);
    clock.setTime();

    timer = millis();

    if (isRedButtonPressed()) launchMode(configuration);
    else launchMode(standard);
}

#if DEBUG
void loop() {
    // Serial.println("Testing LED...");

    // setLEDColor(RED);
    // Serial.println("RED");
    // delay(DEBUG_DELAY);

    // setLEDColor(GREEN);
    // Serial.println("GREEN");
    // delay(DEBUG_DELAY);

    // setLEDColor(BLUE);
    // Serial.println("BLUE");
    // delay(DEBUG_DELAY);

    // setLEDColor(BLACK);
    // Serial.println("done");


    // Serial.println("\nTesting Light sensor...");
    // for (int i = 0; i < TEST_BATCH; i++) {
    //     delay(DEBUG_DELAY);
    //     Serial.println(getLightSensorValue());
    // }
    // Serial.println("done");


    // Serial.println("Testing buttons...");

    // Serial.println("  Press red button.");
    // while (!isRedButtonPressed());

    // Serial.println("  Press green button.");
    // while (!isGreenButtonPressed());

    // Serial.println("done");


    // Serial.println("Testing GPS...");
    // for (int i = 0; i < TEST_BATCH; i++) {
    //     String gpsData="";
    //     if (serialGPS.available())
    //     {
    //         bool t=true;
    //         while(t){
    //             gpsData = serialGPS.readStringUntil('\n');
    //             if (gpsData.startsWith("$GPGGA")){
    //                 t=false;
    //             }
    //         }
    //     }
    //     Serial.println(gpsData);
    //     delay(DEBUG_DELAY);
    // }
    // Serial.println("done");


    // Serial.println("Testing BME280...");
    // for (int i = 0; i < TEST_BATCH; i++) {
    //     Serial.println(bme.readTemperature());
    //     Serial.println(bme.readPressure() / 100.0F);
    //     Serial.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
    //     Serial.println(bme.readHumidity());

    //     delay(DEBUG_DELAY);
    // }
    // Serial.println("done");


    // Serial.println("Testing complete!");
    // while(true);
}
#else
void loop() {
    if (isRedButtonPressed()) interruptRed();
    if (isGreenButtonPressed()) interruptGreen();

    String readings = "";
    if (currentMode == configuration) {
        if (isElapsed(timer, 30*60)) launchMode(standard);
        else configCmdHandler();
    } else {
        if (isElapsed(timer, getConfig(LOG_INTERVAL)*1000)) {
            // horodatage
            readings = getDateTime() + " | ";

            // pression
            if (getConfig(PRESSURE)) readings += String(bme.readPressure() / 100.0F) +  "hPa | ";
            // temperature
            if (getConfig(TEMP_AIR)) readings += String(bme.readTemperature()) + " °C | ";
            // hygrometrie
            if (getConfig(HYGR)) readings += String(bme.readHumidity()) + "% | ";

            // luminosite
            if (getConfig(LUMIN)) readings += "Lux: " + String(getLightSensorValue()) + " | ";

            // GPS
            if (currentMode == economique) {
                gpsToggle = !gpsToggle;
                if (!gpsToggle) return;
            };
            String gpsData;
            if (serialGPS.available())
            {
                do {
                    gpsData = serialGPS.readStringUntil('\n');
                } while (!gpsData.startsWith("$GPGGA"));
            }
            readings += gpsData + " |";
            if (!writeOnSdFile("datalog.txt", readings)) launchErrorSequence(sdAccessDenied, true);
            Serial.println(readings);
            timer = millis();
        }
    }
}
#endif