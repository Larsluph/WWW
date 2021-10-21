#include "utils.h"
#include <EEPROM.h>

bool isElapsed(unsigned long time, unsigned long cd) {
    return (millis() - time > cd);
}

uint8_t getYear() {
    return clock.year;
}
uint8_t getMonth() {
    return clock.month;
}
uint8_t getDay() {
    return clock.dayOfMonth;
}
uint8_t getHours() {
    return clock.hour;
}
uint8_t getMinutes() {
    return clock.minute;
}
uint8_t getSeconds() {
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
    SdVolume volume;
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

bool isRedButtonPressed() { return (digitalRead(PIN_BUTTON_RED) == LOW) ? true : false; }
bool isGreenButtonPressed() { return (digitalRead(PIN_BUTTON_GREEN) == LOW) ? true : false; }

void setLEDColor(Color color) {
    led.setColorRGB(0, color.r, color.g, color.b);
}

int getLightSensorValue() {
    return analogRead(PIN_LIGHT_SENSOR);
}

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
        timer = millis();
    }
}
void configCmdHandler(String cmd) {
    if (cmd == "RESET") restoreConfig();
    else if (cmd == "VERSION") Serial.println("v1.0.0, lot n°0001");
    else {
        int value = cmd.substring(cmd.indexOf('=')+1).toInt();
        if (value == 0) return;

        if (cmd.startsWith("LOG_INTERVAL="))       saveConfig(LOG_INTERVAL, value);
        else if (cmd.startsWith("FILE_MAX_SIZE=")) saveConfig(FILE_MAX_SIZE, value);
        else if (cmd.startsWith("TIMEOUT="))       saveConfig(TIMEOUT, value);
        else if (cmd.startsWith("LUMIN="))         saveConfig(LUMIN, value);
        else if (cmd.startsWith("LUMIN_LOW="))     saveConfig(LUMIN_LOW, value);
        else if (cmd.startsWith("LUMIN_HIGH="))    saveConfig(LUMIN_HIGH, value);
        else if (cmd.startsWith("TEMP_AIR="))      saveConfig(TEMP_AIR, value);
        else if (cmd.startsWith("MIN_TEMP_AIR="))  saveConfig(MIN_TEMP_AIR, value);
        else if (cmd.startsWith("MAX_TEMP_AIR="))  saveConfig(MAX_TEMP_AIR, value);
        else if (cmd.startsWith("HYGR="))          saveConfig(HYGR, value);
        else if (cmd.startsWith("HYGR_MINT="))     saveConfig(HYGR_MINT, value);
        else if (cmd.startsWith("HYGR_MAXT="))     saveConfig(HYGR_MAXT, value);
        else if (cmd.startsWith("PRESSURE="))      saveConfig(PRESSURE, value);
        else if (cmd.startsWith("PRESSURE_MIN="))  saveConfig(PRESSURE_MIN, value);
        else if (cmd.startsWith("PRESSURE_MAX="))  saveConfig(PRESSURE_MAX, value);
        else if (cmd.startsWith("CLOCK="))         saveConfig(CLOCK, value);
        else if (cmd.startsWith("DATE="))          saveConfig(DATE, value);
        else if (cmd.startsWith("DAY="))           saveConfig(DAY, value);
    }
}
