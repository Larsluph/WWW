#include "utils.h"
#include <EEPROM.h>

void block() {
    while (1);
}

bool isElapsed(unsigned long time, int cd) {
    return (millis() - time > cd);
}

uint32_t getSizeLeft() {
    // Initializing SD

    Sd2Card card;
    if (!card.init(SPI_FULL_SPEED, PIN_SD)) {
        // TODO: error opening SD Card
        // card.errorCode() == SD_CARD_ERROR_CMD0
        // -> timeout
        Serial.println("Can't detect SD Card!");
        block();
    }

    // Initializing volume
    SdVolume volume;
    if (!volume.init(&card)) {
        // TODO: error opening volume (card not formatted ?)
        Serial.println("Error opening volume.");
        block();
    }

    SdFile root;
    root.openRoot(&volume);
    uint32_t usedSpace = root.fileSize();
    root.close();

    uint32_t volumesize = (volume.blocksPerCluster() * volume.clusterCount()) / 2;

    return volumesize - usedSpace;
}

bool isRedButtonPressed() {
    if (digitalRead(PIN_BUTTON_RED) == 0) return true;
    else {
        lastRedPressed = millis();
        consumed = false;
        return false;
    }
}
bool isRedButtonLongPressed() {
    if (isRedButtonPressed() && (millis() - lastRedPressed > 5000) && !consumed) {
        consumed = true;
        return true;
    } else return false;
}

bool isGreenButtonPressed() {
    if (digitalRead(PIN_BUTTON_GREEN) == 0) return true;
    else {
        lastGreenPressed = millis();
        consumed = false;
        return false;
    }
}
bool isGreenButtonLongPressed() {
    if (isGreenButtonPressed() && (millis() - lastGreenPressed > 5000) && !consumed) {
        consumed = true;
        return true;
    } else return false;
}

void setLEDColor(Color &color) {
    led.setColorRGB(0, color.r, color.g, color.b);
}

int getLightSensorValue() {
    return analogRead(PIN_LIGHT_SENSOR);
}

String getGpsData() {
    String result = "";
    if (serialGPS.available()) {
        result = serialGPS.readStringUntil('\n');
    }
    return result;
}

float getTemperature() { return bme.readTemperature(); }
float getPressure() { return bme.readPressure() / 100.0F; }
float getAltitude() { return bme.readAltitude(1013.25); }
float getHumidity() { return bme.readHumidity(); }

int getConfig(ConfigID id) { return EEPROM.read(id); }
void saveConfig(ConfigID id, int newValue) { EEPROM.update(id, newValue); }
void restoreConfig() {
    saveConfig(LOG_INTERVAL, 10);
    saveConfig(FILE_MAX_SIZE, 4096);
    saveConfig(TIMEOUT, 30);
    saveConfig(LUMIN, 1);
    saveConfig(LUMIN_LOW, 255);
    saveConfig(LUMIN_HIGH, 768);
    saveConfig(TEMP_AIR, 1);
    saveConfig(MIN_TEMP_AIR, -10);
    saveConfig(MAX_TEMP_AIR, 60);
    saveConfig(HYGR, 1);
    saveConfig(HYGR_MINT, 0);
    saveConfig(HYGR_MAXT, 50);
    saveConfig(PRESSURE, 1);
    saveConfig(PRESSURE_MIN, 850);
    saveConfig(PRESSURE_MAX, 1080);
}

void configCmdHandler() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        configCmdHandler(cmd);
    }
}
void configCmdHandler(String cmd) {
    if (cmd == "RESET") restoreConfig();
    else if (cmd == "VERSION") Serial.println("v1.0.0, lot n°0001");
    else if (cmd.startsWith("LOG_INTERVAL="));
    else if (cmd.startsWith("FILE_MAX_SIZE="));
    else if (cmd.startsWith("TIMEOUT="));
    else if (cmd.startsWith("LUMIN="));
    else if (cmd.startsWith("LUMIN_LOW="));
    else if (cmd.startsWith("LUMIN_HIGH="));
    else if (cmd.startsWith("TEMP_AIR="));
    else if (cmd.startsWith("MIN_TEMP_AIR="));
    else if (cmd.startsWith("MAX_TEMP_AIR="));
    else if (cmd.startsWith("HYGR="));
    else if (cmd.startsWith("HYGR_MINT="));
    else if (cmd.startsWith("HYGR_MAXT="));
    else if (cmd.startsWith("PRESSURE="));
    else if (cmd.startsWith("PRESSURE_MIN="));
    else if (cmd.startsWith("PRESSURE_MAX="));
    else if (cmd.startsWith("CLOCK="));
    else if (cmd.startsWith("DATE="));
    else if (cmd.startsWith("DAY="));
}
