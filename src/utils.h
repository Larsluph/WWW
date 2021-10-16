#ifndef utils
#define utils

#include <SD.h>
#include <SoftwareSerial.h>
#include <ChainableLED.h>
#include <Adafruit_BME280.h>
#include <DS1307.h>

#include "pins.h"
#include "enums.h"
#include "modes.h"

#define SEALEVELPRESSURE_HPA 1013.25

static SoftwareSerial serialGPS(PIN_GPS_RX, PIN_GPS_TX);
static ChainableLED led(PIN_LED_CLK, PIN_LED_DATA, 1);
static Adafruit_BME280 bme;
static DS1307 clock;
static Sd2Card card;
static SdVolume volume;


// returns whether {cd} ms has elapsed since {time}
bool isElapsed(unsigned long, int);


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
void setLEDColor(Color);


// returns light level from sensor [0, 800]
int getLightSensorValue();


// reads config byte from EEPROM
int getConfig(ConfigID);
// updates config byte in EEPROM
void saveConfig(ConfigID, int);
// restore default config values in EPPROM
void restoreConfig();


void configCmdHandler();
void configCmdHandler(String);

#endif