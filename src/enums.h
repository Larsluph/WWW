#ifndef enums
#define enums

#include <Arduino.h>

// available modes for WWW
enum Mode {
    standard,
    configuration,
    maintenance,
    economique
};

// error codes for error sequences
enum ErrorCodes {
    rtcClockFetchError,
    gpsFetchError,
    probeFetchError,
    probeInconsistentData,
    sdStorageFull,
    sdAccessDenied
};

// config byte address in EEPROM
enum ConfigID {
    LOG_INTERVAL,
    FILE_MAX_SIZE,
    TIMEOUT,
    LUMIN,
    LUMIN_LOW,
    LUMIN_HIGH,
    TEMP_AIR,
    MIN_TEMP_AIR,
    MAX_TEMP_AIR,
    HYGR,
    HYGR_MINT,
    HYGR_MAXT,
    PRESSURE,
    PRESSURE_MIN,
    PRESSURE_MAX,
    CLOCK,
    DATE,
    DAY
};

// represents a color to use with RGB LED
struct Color {
    byte r;
    byte g;
    byte b;

    Color operator|(Color &other) {
        Color result;

        result.r = this->r | other.r;
        result.g = this->g | other.g;
        result.b = this->b | other.b;

        return result;
    }
};

#endif