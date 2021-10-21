#include "modes.h"
#include "colors.h"
#include "utils.h"
#include "enums.h"

void launchMode(Mode newMode) {
    Serial.print("Switching to mode ");
    Serial.println(newMode);
    prevMode = currentMode;
    currentMode = newMode;
    timer = millis();
    switch(currentMode) {
        case standard:
            setLEDColor(GREEN);
            break;
        case economique:
            setLEDColor(BLUE);
            gpsToggle = true;
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
    Serial.println("interrupt red!");
    lastPress = millis();
    while (isRedButtonPressed()) {
        if (isElapsed(lastPress, 5000)) {
            if (currentMode == standard || currentMode == economique) launchMode(maintenance);
            else if (currentMode == maintenance) launchMode(prevMode);

            return;
        }
    }
}
void interruptGreen() {
    Serial.println("interrupt green!");
    lastPress = millis();
    while (isGreenButtonPressed()) {
        if (isElapsed(lastPress, 5000)) {
            if (currentMode == standard) launchMode(economique);
            else if (currentMode == economique) launchMode(standard);

            return;
        }
    }
}

void iterErrorSequence(Color a, Color b, bool sub) {
    if (isElapsed(timer, frequence)) {
        timer = millis();
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
