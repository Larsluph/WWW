#include "utils.h"
#include "pins.h"
#include "colors.h"
#include "modes.h"

#define DEBUG 0
#define DEBUG_DELAY 1000
#define TEST_BATCH 3
#define ENABLE_READINGS 1

Mode prevMode = standard;
Mode currentMode = standard;

unsigned long lastPress = millis();

bool intermittence = false;
bool subIntermittence = false;
unsigned long timer = millis();

bool gpsToggle = true;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Starting WWW...");

    restoreConfig();

    serialGPS.begin(9600);

    // setup buttons
    pinMode(PIN_BUTTON_RED, INPUT);
    pinMode(PIN_BUTTON_GREEN, INPUT);

    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_RED), interruptRed, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_GREEN), interruptGreen, FALLING);

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
    clock.fillByHMS(10, 30, 0);
    clock.fillDayOfWeek(FRI);
    clock.setTime();

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

    switch (currentMode) {
        case configuration:
            if (isElapsed(timer, 5000)) launchMode(standard);
            else configCmdHandler();
            break;
        case maintenance:
            break;
        case standard:
        case economique:
            if (isElapsed(timer, 2000)) {
                #if ENABLE_READINGS
                // horodatage
                String dt = getDateTime();
                Serial.print(dt);
                Serial.print(" | ");

                // pression
                Serial.print(bme.readPressure() / 100.0F);
                Serial.print("hPa | ");

                // temperature
                Serial.print(bme.readTemperature());
                Serial.print(" °C | ");

                // hygrometrie
                Serial.print(bme.readHumidity());
                Serial.print("% | ");

                // luminosite
                Serial.print(getLightSensorValue());
                Serial.print(" | ");

                // GPS
                while (true) {
                    String data;
                    while (serialGPS.available()) {
                        data = serialGPS.readStringUntil('\n');
                    }
                    int gpgga_index = data.lastIndexOf("$GPGGA");
                    if (gpgga_index != -1) {
                        Serial.print(data.substring(gpgga_index, data.length()-1));
                        Serial.println(" |");
                        break;
                    }
                    else {
                        Serial.println("#NA |");
                    }
                }

                Serial.println();
                // if (!writeOnSdFile("datalog.txt", readings)) launchErrorSequence(sdAccessDenied, true);
                #endif
                timer = millis();
            }
    }
}
#endif