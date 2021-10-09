#ifndef enums
#define enums

#include <Arduino.h>

enum Mode {
    standard,
    configuration,
    maintenance,
    economique
};

enum ErrorCodes {
    rtcClockFetchError,
    gpsFetchError,
    probeFetchError,
    probeInconsistentData,
    sdStorageFull,
    sdAccessDenied
};

enum ConfigID {
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
    PRESSURE_MAX
};

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