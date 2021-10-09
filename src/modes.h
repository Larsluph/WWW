#ifndef modes
#define modes

#include "pins.h"
#include "enums.h"

static int currentMode = 0;

#define frequence 1000

static bool intermittence = false;
static bool subIntermittence = false;
static unsigned long lastToggle = millis();
void launchErrorSequence(ErrorCodes);

void switchTo(int, int);
void switchTo(Mode, Mode);

void endMode(Mode);
void launchMode(Mode);

#endif