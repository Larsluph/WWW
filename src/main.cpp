#include "utils.h"
#include "pins.h"
#include "colors.h"
#include "modes.h"

#define DEBUG 0
#define DEBUG_DELAY 1000
#define TEST_BATCH 3
#define ENABLE_READINGS 1

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Starting WWW...");

    restoreConfig();

    serialGPS.begin(9600);

    // setup buttons
    pinMode(PIN_BUTTON_RED, INPUT);
    pinMode(PIN_BUTTON_GREEN, INPUT);

    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_RED), interruptRed, CHANGE);
    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_GREEN), interruptGreen, CHANGE);

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

    if (currentMode == configuration) {
        if (isElapsed(timer, 1000)) launchMode(standard);
        else configCmdHandler();
    } else {
        if (isElapsed(timer, getConfig(LOG_INTERVAL)*1000)) {
            Serial.println("reading probes...");

            #if ENABLE_READINGS
            String readings = "";

            // horodatage
            String dt = getDateTime();
            if (dt) readings += dt + " | ";
            else readings += "#NA | ";

            // pression
            if (getConfig(PRESSURE)) {
                String pressure = String(bme.readPressure() / 100.0F);
                if (pressure) readings += pressure + "hPa | ";
                else readings += "#NA | ";
            }
            // temperature
            if (getConfig(TEMP_AIR)) {
                String temp = String(bme.readTemperature());
                if (temp) readings += temp + " °C | ";
                else readings += "#NA | ";
            }
            // hygrometrie
            if (getConfig(HYGR)) {
                String hygr = String(bme.readHumidity());
                if (hygr) readings += hygr + "% | ";
                else readings += "#NA | ";
            }

            // luminosite
            if (getConfig(LUMIN)) {
                readings += "Lux: ";
                String lum = String(getLightSensorValue());
                if (lum) readings += lum + " | ";
                else readings += "#NA | ";
            }

            // GPS
            if (currentMode == economique) {
                gpsToggle = !gpsToggle;
                if (!gpsToggle) return;
            };
            String gpsData = "";
            if (serialGPS.available())
            {
                bool t = true;
                while (t) {
                    String data = serialGPS.readString();
                    int gpgga_index = data.lastIndexOf("$GPGGA");
                    int gpgga_end_index = data.indexOf('\n');
                    int gpgga_end_index2 = data.indexOf('\n');
                    if (gpgga_end_index != -1 && gpgga_end_index < gpgga_end_index2)
                        String gpgga = data.substring(gpgga_index, gpgga_end_index);
                    else
                        String gpgga = data.substring(gpgga_index, gpgga_end_index2);
                    if (gpsData.startsWith("$GPGGA") || isElapsed(timer, 2000)) t = false;
                }
            }

            if (gpsData.startsWith("$GPGGA")) readings += gpsData + " |";
            else readings += "#NA |";

            Serial.println(readings);
            // if (!writeOnSdFile("datalog.txt", readings)) launchErrorSequence(sdAccessDenied, true);
            #endif
            timer = millis();
        }
    }
}
#endif