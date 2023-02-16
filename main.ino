#include "src/engine.hpp"
#include "src/buttons.hpp"
#include "src/sonar.hpp"
#include "src/grapper.hpp"
#include "src/linesensor.hpp"

char* lineDirection = "none";
long nextLineDetection = millis();
uint16_t sensorValues[8];
bool shouldDrive = false;

void avoidObstacle() {
    drive(0, 0);
    delay(250);
    rotate(90);
    drive(255, 255);
    delay(250);
    drive(0, 0);
    openGrapper();
    delay(250);
    drive(-255, -255);
    delay(250);
    rotate(-90);
    while (!isBlocked(200)) {
        drive(255, 255);
    }
    drive(0, 0);
    delay(250);
    closeGrapper();
    delay(250);
    rotate(-90);
    drive(255, 255);
    delay(250);
    drive(0, 0);
    openGrapper();
    delay(250);
    drive(-255, -255);
    delay(250);
    rotate(180);
    drive(255, 255);
    delay(250);
    drive(0, 0);
    closeGrapper();
    delay(250);
    drive(-255, -255);
    delay(250);
    rotate(-90);
    drive(255, 255);
}

void followLine() {
    uint16_t position = qtr.readLineBlack(sensorValues);
    bool isOnLine = false;
    for (int i = 0; i < 8; i++) {
        if (sensorValues[i] > 975) {
            isOnLine = true;
            break;
        }
    }
    if (isOnLine) {
        if (position > 2500 && position < 4500) {
            drive(255, 255);
        } else {
            drive((position < 2500) ? min(max((int)(255 * (3500 - position) / 3500), 0), 255) : 255, (position > 4500) ? min(max((int)(255 * (position - 3500) / 3500), 0), 255) : 255);
        }
        if (position < 1000) {
            lineDirection = "left";
            nextLineDetection = millis() + 1000;
        } else if (position > 6000) {
            lineDirection = "right";
            nextLineDetection = millis() + 1000;
        } else if (nextLineDetection < millis()) {
            lineDirection = "none";
        }
    } else {
        if (lineDirection == "left") {
            drive(25, 255);
        } else if (lineDirection == "right") {
            drive(255, 25);
        }
    }
}

void driveOnButtonPress() {
    if (isButtonPressed()) {
        shouldDrive = !shouldDrive;
        if (shouldDrive) {
            drive(255, 255);
        } else {
            drive(0, 0);
        }
    }
    if (shouldDrive) {
        if (isBlocked()) {
            avoidObstacle();
        } else {
            followLine();
        }
    }
}

void setup() {
    initButtons();
    initEngine();
    initGrapper();
    initLineSensor();
    initSonar();
}

void loop() {
    if (!grapperClosed) {
        closeGrapper();
    }
    driveOnButtonPress();
}