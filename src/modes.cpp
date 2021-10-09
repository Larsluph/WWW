#include "modes.h"
#include "colors.h"
#include "utils.h"
#include "enums.h"

void switchTo(int prevMode, int newMode) { switchTo((Mode)prevMode, (Mode)newMode); }
void switchTo(Mode prevMode, Mode newMode) {
    endMode(prevMode);
    launchMode(newMode);
}

void endMode(Mode currentMode) {
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
    // if (isRedButtonLongPressed()) {}
    // else if (isGreenButtonLongPressed()) {}

    switch(newMode) {
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

void launchErrorSequence(ErrorCodes err, bool block) {
    if (block)
        while (true) { launchErrorSequence(err); }
    else
        launchErrorSequence(err);
}
void launchErrorSequence(ErrorCodes err) {
    switch (err) {
        case rtcClockFetchError:
            if (millis() - lastToggle >= frequence) {
                lastToggle = millis();
                if (intermittence) {
                    setLEDColor(BLUE);
                } else {
                    setLEDColor(RED);
                }
                intermittence = !intermittence;
            }
            break;
        case gpsFetchError:
            if (millis() - lastToggle >= frequence) {
                lastToggle = millis();
                if (intermittence) {
                    setLEDColor(YELLOW);
                } else {
                    setLEDColor(RED);
                }
                intermittence = !intermittence;
            }
            break;
        case probeFetchError:
            if (millis() - lastToggle >= frequence) {
                lastToggle = millis();
                if (intermittence) {
                    setLEDColor(GREEN);
                } else {
                    setLEDColor(RED);
                }
                intermittence = !intermittence;
            }
            break;
        case probeInconsistentData:
            if (millis() - lastToggle >= frequence) {
                lastToggle = millis();
                if (intermittence) {
                    setLEDColor(GREEN);
                } else {
                    setLEDColor(RED);
                }

                if (intermittence && subIntermittence) {
                    intermittence = !intermittence;
                    subIntermittence = !subIntermittence;
                } else if (intermittence) {
                    subIntermittence = !subIntermittence;
                } else {
                    intermittence = !intermittence;
                }
            }
            break;
        case sdStorageFull:
            if (millis() - lastToggle >= frequence) {
                lastToggle = millis();
                if (intermittence) {
                    setLEDColor(WHITE);
                } else {
                    setLEDColor(RED);
                }
                intermittence = !intermittence;
            }
            break;
        case sdAccessDenied:
            if (millis() - lastToggle >= frequence) {
                lastToggle = millis();
                if (intermittence) {
                    setLEDColor(WHITE);
                } else {
                    setLEDColor(RED);
                }
                
                if (intermittence && subIntermittence) {
                    intermittence = !intermittence;
                    subIntermittence = !subIntermittence;
                } else if (intermittence) {
                    subIntermittence = !subIntermittence;
                } else {
                    intermittence = !intermittence;
                }
            }
            break;
        default:
            break;
    }
}
