#include "Servo.h"

Servo grapper;
bool grapperClosed = false;
const int grapperPin = 2;

void initGrapper() {
    grapper.attach(grapperPin);
}

void toggleGrapper() {
    grapper.write((grapperClosed) ? 90 : 45);
    grapperClosed = !grapperClosed;
}

void openGrapper() {
    grapper.write(90);
    grapperClosed = false;
}

void closeGrapper() {
    grapper.write(45);
    grapperClosed = true;
}