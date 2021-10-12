#ifndef utils
#define utils

#include <SD.h>
#include <SoftwareSerial.h>
#include <ChainableLED.h>
#include <Adafruit_BME280.h>

#include "pins.h"
#include "enums.h"
#include "modes.h"

static SoftwareSerial serialGPS(PIN_GPS_RX, PIN_GPS_TX);
static ChainableLED led(PIN_LED_CLK, PIN_LED_DATA, 1);
static Adafruit_BME280 bme;
static Sd2Card card;
static SdVolume volume;

// infinite loop doing nothing
void block();

// returns whether {cd} ms has elapsed since {time}
bool isElapsed(unsigned long, int);

// returns size left on SD card (in kB)
uint32_t getSizeLeft();
bool writeOnSdFile(String, String);


static bool consumed = false;
static long lastRedPressed = millis();
static long lastGreenPressed = millis();

// returns whether red button is pressed or not
bool isRedButtonPressed();
// returns whether red button is pressed for 5 secs
bool isRedButtonLongPressed();

// returns whether green button is pressed or not
bool isGreenButtonPressed();
// returns whether green button is pressed for 5 secs
bool isGreenButtonLongPressed();


// send cmd to LED to change displayed color
void setLEDColor(Color);


// returns light level from sensor [0, 800]
int getLightSensorValue();


// returns raw string formatted by GPS
String getGpsData();


// returns temperature from probe [-40, 85] (in °C)
float getTemperature();
// returns pressure from probe [300, 1100] (in hPa)
float getPressure();
// returns approximate altitude (in meters)
float getAltitude();
// returns humidity from probe (+/- 3%) (in %)
float getHumidity();


// reads config byte from EEPROM
int getConfig(ConfigID);
// updates config byte in EEPROM
void saveConfig(ConfigID, int);
// restore default config values in EPPROM
void restoreConfig();


void configCmdHandler();
void configCmdHandler(String);

#endif