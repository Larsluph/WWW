#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <ChainableLED.h>
#include <DS1307.h>
#include <EEPROM.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define SEALEVEL12_HPA 1013.25

#define DEBUG 0
#define DEBUG_DELAY 1000
#define TEST_BATCH 3
#define ENABLE_READINGS 1

#define PIN_LIGHT_SENSOR A0

#define PIN_BUTTON_RED 2
#define PIN_BUTTON_GREEN 3
#define PIN_SD 4
#define PIN_GPS_RX 5
#define PIN_GPS_TX 6
#define PIN_LED_CLK 8
#define PIN_LED_DATA 9

#define ADDR_BME280 0x76

SoftwareSerial serialGPS(PIN_GPS_RX, PIN_GPS_TX);
ChainableLED led(PIN_LED_CLK, PIN_LED_DATA, 1);
Adafruit_BME280 bme;
DS1307 clock;
Sd2Card card;
SdVolume volume;

byte prevMode = 0;
byte currentMode = 0;
unsigned long lastPress = millis();
const int frequence = 1000;
bool intermittence = false;
bool subIntermittence = false;
unsigned long timer = millis();
bool gpsToggle = true;

void launchMode(byte);

void interruptRed();
void interruptGreen();

void iterErrorSequence(byte, byte, byte, byte, byte, byte, bool);
void launchErrorSequence(byte, bool);
void launchErrorSequence(byte);

// returns whether {cd} ms has elapsed since {time}
bool isElapsed(unsigned long, unsigned long);


int getYear();
int getMonth();
int getDay();
int getHours();
int getMinutes();
int getSeconds();
String getWeekDay();
String getDate();
String getTime();
String getDateTime();


// returns size left on SD card (in kB)
uint32_t getSizeLeft();
bool writeOnSdFile(String, String);


// returns whether red button is pressed or not
bool isRedButtonPressed();
// returns whether green button is pressed or not
bool isGreenButtonPressed();


// send cmd to LED to change displayed color
void setLEDColor(byte, byte, byte);


// returns light level from sensor [0, 800]
int getLightSensorValue();


// reads config byte from EEPROM
int getConfig(byte);
// updates config byte in EEPROM
void saveConfig(byte, int);
// restore default config values in EPPROM
void restoreConfig();


void configCmdHandler();
void configCmdHandler(String);

bool isElapsed(unsigned long time, unsigned long cd) {
    return (millis() - time > cd);
}

int getYear() {
    return clock.year;
}
int getMonth() {
    return clock.month;
}
int getDay() {
    return clock.dayOfMonth;
}
int getHours() {
    return clock.hour;
}
int getMinutes() {
    return clock.minute;
}
int getSeconds() {
    return clock.second;
}
String getWeekDay() {
    switch (clock.dayOfWeek) {
        case MON:
            return "MON";
        case TUE:
            return "TUE";
        case WED:
            return "WED";
        case THU:
            return "THU";
        case FRI:
            return "FRI";
        case SAT:
            return "SAT";
        case SUN:
            return "SUN";
        default:
            return "#NA";
    }
}

String getDate() {
    clock.getTime();
    String result = getWeekDay();
    result += " ";
    result += getYear();
    result += "-";
    result += getMonth();
    result += "-";
    result += getDay();
    return result;
}
String getTime() {
    clock.getTime();
    String result;
    result += getHours();
    result += ":";
    result += getMinutes();
    result += ":";
    result += getSeconds();
    return result;
}
String getDateTime() {
    return getDate() + " " + getTime();
}

uint32_t getSizeLeft() {
    SdFile root;
    root.openRoot(&volume);
    uint32_t usedSpace = root.fileSize();
    root.close();

    uint32_t volumesize = (volume.blocksPerCluster() * volume.clusterCount()) / 2;

    return volumesize - usedSpace;
}

bool writeOnSdFile(String filename, String data) {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
        file.println(data);
        file.close();
        return true;
    } else return false;
}

bool isRedButtonPressed() { return (digitalRead(PIN_BUTTON_RED) == 0) ? true : false; }
bool isGreenButtonPressed() { return (digitalRead(PIN_BUTTON_GREEN) == 0) ? true : false; }

void setLEDColor(byte r, byte g, byte b) {
    led.setColorRGB(0, r, g, b);
}

int getLightSensorValue() {
    return analogRead(PIN_LIGHT_SENSOR);
}

int getConfig(byte id) { return EEPROM.read(id); }
void saveConfig(byte id, int newValue) { EEPROM.update(id, newValue); }
void restoreConfig() {
    saveConfig(0, 10);
    saveConfig(1, 4096);
    saveConfig(2, 30);
    saveConfig(3, 1);
    saveConfig(4, 255);
    saveConfig(5, 768);
    saveConfig(6, 1);
    saveConfig(7, -10);
    saveConfig(8, 60);
    saveConfig(9, 1);
    saveConfig(10, 0);
    saveConfig(11, 50);
    saveConfig(12, 1);
    saveConfig(13, 850);
    saveConfig(14, 1080);
}

void configCmdHandler() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        configCmdHandler(cmd);
        timer = millis();
    }
}
void configCmdHandler(String cmd) {
    if (cmd == "RESET") restoreConfig();
    else if (cmd == "VERSION") Serial.println("v1.0.0, lot n°0001");
    else {
        int value = cmd.substring(cmd.indexOf('=')+1).toInt();
        if (value == 0) return;

        if (cmd.startsWith("0="))       saveConfig(0, value);
        else if (cmd.startsWith("1=")) saveConfig(1, value);
        else if (cmd.startsWith("2="))       saveConfig(2, value);
        else if (cmd.startsWith("3="))         saveConfig(3, value);
        else if (cmd.startsWith("4="))     saveConfig(4, value);
        else if (cmd.startsWith("5="))    saveConfig(5, value);
        else if (cmd.startsWith("6="))      saveConfig(6, value);
        else if (cmd.startsWith("7="))  saveConfig(7, value);
        else if (cmd.startsWith("8="))  saveConfig(8, value);
        else if (cmd.startsWith("9="))          saveConfig(9, value);
        else if (cmd.startsWith("10="))     saveConfig(10, value);
        else if (cmd.startsWith("11="))     saveConfig(11, value);
        else if (cmd.startsWith("12="))      saveConfig(12, value);
        else if (cmd.startsWith("13="))  saveConfig(13, value);
        else if (cmd.startsWith("14="))  saveConfig(14, value);
        else if (cmd.startsWith("15="))         saveConfig(15, value);
        else if (cmd.startsWith("16="))          saveConfig(16, value);
        else if (cmd.startsWith("17="))           saveConfig(17, value);
    }
}

void launchMode(byte newMode) {
    Serial.print("Switching to mode ");
    Serial.println(newMode);
    prevMode = currentMode;
    currentMode = newMode;
    timer = millis();
    switch(currentMode) {
        case 0:
            setLEDColor(0, 255, 0);
            break;
        case 1:
            setLEDColor(0, 0, 255);
            gpsToggle = true;
            break;
        case 2:
            setLEDColor(255, 107, 0);
            break;
        case 3:
            setLEDColor(255, 255, 0);
            break;
        default:
            break;
    }
}

void interruptRed() {
    Serial.println("interrupt red!");
    lastPress = millis();
    while (isRedButtonPressed()) {
        if (isElapsed(lastPress, 5000)) {
            if (currentMode == 0 || currentMode == 1) launchMode(2);
            else if (currentMode == 2) launchMode(prevMode);

            return;
        }
    }
}
void interruptGreen() {
    Serial.println("interrupt green!");
    lastPress = millis();
    while (isGreenButtonPressed()) {
        if (isElapsed(lastPress, 5000)) {
            if (currentMode == 0) launchMode(1);
            else if (currentMode == 1) launchMode(0);

            return;
        }
    }
}

void iterErrorSequence(byte ar, byte ag, byte ab, byte br, byte bg, byte bb, bool sub) {
    if (millis() - timer >= frequence) {
        timer = millis();
        if (intermittence) {
            setLEDColor(br, bg, bb);
        } else {
            setLEDColor(ar, ag, ab);
        }

        if (sub) {
            if (intermittence && subIntermittence) {
                intermittence = !intermittence;
                subIntermittence = !subIntermittence;
            } else if (intermittence) {
                subIntermittence = !subIntermittence;
            } else {
                intermittence = !intermittence;
            }
        } else intermittence = !intermittence;
    }
}
void launchErrorSequence(byte err, bool block) {
    if (block)
        while (true) { launchErrorSequence(err); }
    else
        launchErrorSequence(err);
}
void launchErrorSequence(byte err) {
    switch (err) {
        case 0:
            iterErrorSequence(255, 0, 0, 0, 0, 255, false);
            break;
        case 1:
            iterErrorSequence(255, 0, 0, 255, 255, 0, false);
            break;
        case 2:
            iterErrorSequence(255, 0, 0, 0, 255, 0, false);
            break;
        case 3:
            iterErrorSequence(255, 0, 0, 0, 255, 0, true);
            break;
        case 4:
            iterErrorSequence(255, 0, 0, 255, 255, 255, false);
            break;
        case 5:
            iterErrorSequence(255, 0, 0, 255, 255, 255, true);
            break;
        default:
            break;
    }
}

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
    setLEDColor(0, 0, 0);

    if (!SD.begin(PIN_SD)) {
        Serial.println("Can't detect SD Card!");
        launchErrorSequence(5, true);
    }

    // setup bme (temp, pressure, humidity)
    if (!bme.begin(ADDR_BME280)) {
        Serial.println("BME280 initialization error!");
        launchErrorSequence(2, true);
    }

    clock.begin();
    clock.fillByYMD(2021, 10, 22);
    clock.fillByHMS(10, 0, 0);
    clock.fillDayOfWeek(FRI);
    clock.setTime();

    timer = millis();

    if (isRedButtonPressed()) launchMode(3);
    else launchMode(0);
}

#if DEBUG
void loop() {
    // Serial.println("Testing LED...");

    // setLEDColor(255, 0, 0);
    // Serial.println("RED");
    // delay(DEBUG_DELAY);

    // setLEDColor(0, 255, 0);
    // Serial.println("GREEN");
    // delay(DEBUG_DELAY);

    // setLEDColor(0, 0, 255);
    // Serial.println("BLUE");
    // delay(DEBUG_DELAY);

    // setLEDColor(0, 0, 0);
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
    //     Serial.println(bme.readAltitude(SEALEVEL12_HPA));
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
        case 3:
            if (isElapsed(timer, 5000)) launchMode(0);
            else configCmdHandler();
        case 2:
            break;
        case 0:
        case 1:
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
                // if (serialGPS.available())
                // {
                //     while (true) {
                //         String gpsData = serialGPS.readStringUntil('\n');
                //         if (gpsData.indexOf("$GPGGA") != -1) {
                //             Serial.print(gpsData);
                //             break;
                //         }
                //     }
                // }
                    while (true) {
                        String data;
                        while (serialGPS.available()) {
                            data = serialGPS.readStringUntil('\n');
                        }
                        int gpgga_index = data.lastIndexOf("$GPGGA");
                        if (gpgga_index != -1) {
                            // int gpgga_end_index = data.indexOf('$', gpgga_index);
                            // if (gpgga_end_index != -1)
                            //     Serial.print(data.substring(gpgga_index, gpgga_end_index));
                            // else
                            //     Serial.print(data.substring(gpgga_index));
                            Serial.print(data.substring(gpgga_index, data.length()-1));
                            Serial.println(" |");
                            break;
                        }
                    }

                Serial.println();
                // if (!writeOnSdFile("DATALOG.TXT", readings)) launchErrorSequence(5, true);
                #endif
                timer = millis();
            }
    }
}
#endif
