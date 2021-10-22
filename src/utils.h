#ifndef utils
#define utils

#include <stdlib.h>
#include <string.h>

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

// returns whether {cd} ms has elapsed since {time}
bool isElapsed(unsigned long, unsigned long);


uint8_t getYear();
uint8_t getMonth();
uint8_t getDay();
uint8_t getHours();
uint8_t getMinutes();
uint8_t getSeconds();
const char * getWeekDay();
char *getDate();
char *getTime();
char *getDateTime();


// returns size left on SD card (in kB)
uint32_t getSizeLeft();
bool writeOnSdFile(char *, char *);


// returns whether red button is pressed or not
bool isRedButtonPressed();
// returns whether green button is pressed or not
bool isGreenButtonPressed();


// send cmd to LED to change displayed color
void setLEDColor(Color);


void fetchDateTime(char *);
void fetchPression(float *);
void fetchTemperature(float *);
void fetchHumidity(float *);
void fetchLightLevel(int *);
void fetchGPS(char *);

void fetchSensorData(Reading *);


void printToSerial(Reading *);


// reads config byte from EEPROM
int getConfig(ConfigID);
// updates config byte in EEPROM
void saveConfig(ConfigID, int);
// restore default config values in EPPROM
void restoreConfig();


void configCmdHandler();
void configCmdHandler(char *);

#endif