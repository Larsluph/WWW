#ifndef modes
#define modes

#include "pins.h"
#include "enums.h"
#include "utils.h"

extern Mode prevMode;
extern Mode currentMode;

extern unsigned long lastPress;

const int frequence = 1000;
extern bool intermittence;
extern bool subIntermittence;
extern unsigned long timer;

extern bool gpsToggle;

void launchMode(Mode);

void interruptRed();
void interruptGreen();

void iterErrorSequence(Color, Color, bool);
void launchErrorSequence(ErrorCodes, bool);
void launchErrorSequence(ErrorCodes);

#endif