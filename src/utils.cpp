#include "utils.h"
#include <EEPROM.h>

void block() {
    while (1);
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
    return digitalRead(PIN_BUTTON_RED) == 0;
}
bool isGreenButtonPressed() {
    return digitalRead(PIN_BUTTON_GREEN) == 0;
}

void setLEDColor(Color color) {
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


int getConfig(ConfigID id) { return EEPROM.read(id); }
void saveConfig(ConfigID id, int newValue) { EEPROM.update(id, newValue); }
void restoreConfig() {
    saveConfig(ConfigID::LUMIN, 1);
    saveConfig(ConfigID::LUMIN_LOW, 255);
    saveConfig(ConfigID::LUMIN_HIGH, 768);
    saveConfig(ConfigID::TEMP_AIR, 1);
    saveConfig(ConfigID::MIN_TEMP_AIR, -10);
    saveConfig(ConfigID::MAX_TEMP_AIR, 60);
    saveConfig(ConfigID::HYGR, 1);
    saveConfig(ConfigID::HYGR_MINT, 0);
    saveConfig(ConfigID::HYGR_MAXT, 50);
    saveConfig(ConfigID::PRESSURE, 1);
    saveConfig(ConfigID::PRESSURE_MIN, 850);
    saveConfig(ConfigID::PRESSURE_MAX, 1080);
}


void configCmdHandler() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        configCmdHandler(cmd);
    }
}
void configCmdHandler(String cmd) {
    if (cmd == "RESET");
    else if (cmd == "VERSION");
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
