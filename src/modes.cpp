#include "modes.h"
#include "colors.h"
#include "utils.h"
#include "enums.h"

void switchTo(int prevMode, int newMode) { switchTo((Mode)prevMode, (Mode)newMode); }
void switchTo(Mode prevMode, Mode newMode) {
    endMode(prevMode);
    launchMode(newMode);
}

void endMode() {
    switch(currentMode) {
        case standard:
            break;
        case economique:
            break;
        case maintenance:
            break;
        case configuration:
            break;
        default:
            break;
    }
}

void launchMode(Mode newMode) {
    currentMode = newMode;
    switch(currentMode) {
        case standard:
            setLEDColor(GREEN);
            break;
        case economique:
            setLEDColor(BLUE);
            break;
        case maintenance:
            setLEDColor(ORANGE);
            break;
        case configuration:
            setLEDColor(YELLOW);
            break;
        default:
            break;
    }
}

void interruptRed() {
    unsigned long lastPress = millis();

    while (isRedButtonPressed()) {
        if (isElapsed(lastPress, 5000)) {
            //TODO: long press handling
            // switchTo according mode

            return;
        }
    }
}
void interruptGreen() {
    unsigned long lastPress = millis();

    while (isRedButtonPressed()) {
        if (isElapsed(lastPress, 5000)) {
            //TODO: long press handling
            // switchTo according mode

            return;
        }
    }
}

void iterErrorSequence(Color a, Color b, bool sub) {
    if (millis() - lastToggle >= frequence) {
        lastToggle = millis();
        if (intermittence) {
            setLEDColor(b);
        } else {
            setLEDColor(a);
        }

        if (sub) {
            if (intermittence && subIntermittence) {
                intermittence = !intermittence;
                subIntermittence = !subIntermittence;
            } else if (intermittence) {
                subIntermittence = !subIntermittence;
            } else {
                intermittence = !intermittence;
            }
        } else intermittence = !intermittence;
    }
}
void launchErrorSequence(ErrorCodes err, bool block) {
    if (block)
        while (true) { launchErrorSequence(err); }
    else
        launchErrorSequence(err);
}
void launchErrorSequence(ErrorCodes err) {
    switch (err) {
        case rtcClockFetchError:
            iterErrorSequence(RED, BLUE, false);
            break;
        case gpsFetchError:
            iterErrorSequence(RED, YELLOW, false);
            break;
        case probeFetchError:
            iterErrorSequence(RED, GREEN, false);
            break;
        case probeInconsistentData:
            iterErrorSequence(RED, GREEN, true);
            break;
        case sdStorageFull:
            iterErrorSequence(RED, WHITE, false);
            break;
        case sdAccessDenied:
            iterErrorSequence(RED, WHITE, true);
            break;
        default:
            break;
    }
}
