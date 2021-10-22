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
const char *getWeekDay() {
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

char *getDate() {
    clock.getTime();
    char result[13];
    char temp[3];
    strcpy(result, getWeekDay());
    strcat(result, " ");
    strcat(result, itoa(getYear(), temp, 10));
    strcat(result, "-");
    strcat(result, itoa(getMonth(), temp, 10));
    strcat(result, "-");
    strcat(result, itoa(getDay(), temp, 10));
    return result;
}
char *getTime() {
    clock.getTime();
    char result[9];
    char temp[3];
    strcpy(result, itoa(getHours(), temp, 10));
    strcat(result, ":");
    strcat(result, itoa(getMinutes(), temp, 10));
    strcat(result, ":");
    strcat(result, itoa(getSeconds(), temp, 10));
    return result;
}
char *getDateTime() {
    char dt[22];
    strcpy(dt, getDate());
    strcat(dt, " ");
    strcat(dt, getTime());
    return dt;
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

bool writeOnSdFile(char *filename, char *data) {
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

void fetchDateTime(char *ptr) {
    strcpy(ptr, getDateTime());
}
void fetchPression(float *ptr) {
    *ptr = bme.readPressure() / 100.0F;
}
void fetchTemperature(float *ptr) {
    *ptr = bme.readTemperature();
}
void fetchHumidity(float *ptr) {
    *ptr = bme.readHumidity();
}
void fetchLightLevel(int *ptr) {
    *ptr = analogRead(PIN_LIGHT_SENSOR);
}
void fetchGPS(char *ptr) {
    while (true) {
        String data;
        while (serialGPS.available()) {
            data = serialGPS.readStringUntil('\n');
        }
        int gpgga_index = data.lastIndexOf("$GPGGA");
        if (gpgga_index != -1) {
            strcpy(ptr, data.substring(gpgga_index, data.length()-1).c_str());
            return;
        }
    }
}

void fetchSensorData(Reading *readings) {
    fetchDateTime(readings->datetime);
    fetchPression(readings->pression);
    fetchTemperature(readings->temperature);
    fetchHumidity(readings->humidity);
    fetchLightLevel(readings->lightLevel);
    fetchGPS(readings->gps);
}

void printToSerial(Reading *readings) {
    Serial.print(readings->datetime);
    Serial.print(" | ");

    // pression
    Serial.print(*readings->pression);
    Serial.print("hPa | ");

    // temperature
    Serial.print(*readings->temperature);
    Serial.print(" *C | ");

    // hygrometrie
    Serial.print(*readings->humidity);
    Serial.print("% | ");

    // luminosite
    Serial.print(*readings->lightLevel);
    Serial.print(" | ");

    // GPS
    Serial.print(readings->gps);
    Serial.println(" |");
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
