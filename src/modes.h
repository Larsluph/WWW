#ifndef modes
#define modes

#include "pins.h"
#include "enums.h"
#include "utils.h"

static Mode prevMode = standard;
static Mode currentMode = standard;

static unsigned long lastPress = millis();

const int frequence = 1000;
static bool intermittence = false;
static bool subIntermittence = false;
static unsigned long timer = millis();

static bool gpsToggle = true;

void launchMode(Mode);

void interruptRed();
void interruptGreen();

void iterErrorSequence(Color, Color, bool);
void launchErrorSequence(ErrorCodes, bool);
void launchErrorSequence(ErrorCodes);

#endif