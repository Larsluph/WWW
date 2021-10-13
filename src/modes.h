#ifndef modes
#define modes

#include "pins.h"
#include "enums.h"
#include "utils.h"

static int currentMode = 0;

const int frequence = 1000;
static bool intermittence = false;
static bool subIntermittence = false;
static unsigned long lastToggle = millis();

void switchTo(int, int);
void switchTo(Mode, Mode);

void endMode(Mode);

void launchMode(Mode);

void interruptRed();
void interruptGreen();

void iterErrorSequence(Color, Color, bool);
void launchErrorSequence(ErrorCodes, bool);
void launchErrorSequence(ErrorCodes);

#endif