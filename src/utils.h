#ifndef utils
#define utils

#include <SD.h>
#include <SoftwareSerial.h>
#include <ChainableLED.h>

#include "pins.h"
#include "enums.h"

static SoftwareSerial serialGPS(PIN_GPS_RX, PIN_GPS_TX);
static ChainableLED led(PIN_LED_CLK, PIN_LED_DATA, 1);

static long lastRedPressed = millis();
static long lastGreenPressed = millis();

void block();

uint32_t getSizeLeft();

bool isRedButtonPressed();
bool isGreenButtonPressed();

void setLEDColor(Color);

int getLightSensorValue();

String getGpsData();

int getConfig(ConfigID);
void saveConfig(ConfigID, int);
void restoreConfig();

void configCmdHandler();
void configCmdHandler(String);

#endif