#include "src/engine.hpp"
#include "src/buttons.hpp"
#include "src/sonar.hpp"
#include "src/grapper.hpp"
#include "src/linesensor.hpp"

char* lineDirection = "none";
long nextLineDetection = millis();
bool shouldDrive = false;

int rotationToObstacle = 0;
long startGoingToObstacle;
long stopGoingToObstacle;

void avoidObstacle() {
    // zet pion rechts
    drive(0, 0);
    delay(200);
    rotate(90);
    drive(0, 0);
    delay(200);
    drive(150, 150);
    delay(250);
    drive(0, 0);
    delay(200);
    openGrapper();
    delay(200);
    // centreer op lijn
    drive(-150, -150);
    delay(350);
    while (!isBlocked()) {
        rotate(-3);
        rotationToObstacle += 3;
    }
    rotate(-10);
    rotationToObstacle += 10;
    drive(0, 0);
    delay(200);
    // pak obstakel
    startGoingToObstacle = millis();
    while (!isBlocked(200)) {
        drive(150, 150);
    }
    stopGoingToObstacle = millis();
    drive(0, 0);
    delay(200);
    closeGrapper();
    delay(200);
    drive(-150, -150);
    delay(stopGoingToObstacle - startGoingToObstacle);
    drive(0, 0);
    // zet obstakel links
    rotate(-90);
    drive(0, 0);
    delay(200);
    drive(150, 150);
    delay(250);
    drive(0, 0);
    delay(200);
    openGrapper();
    delay(200);
    // centreer op lijn
    drive(-150, -150);
    delay(350);
    rotate(90 + rotationToObstacle);
    drive(0, 0);
    delay(200);
    // pak pion op
    drive(150, 150);
    delay(350);
    drive(0, 0);
    delay(200);
    closeGrapper();
    delay(200);
    // centreer op lijn
    drive(-150, -150);
    delay(350);
    rotate(-90);
    drive(0, 0);
    delay(200);
    drive(255, 255);
    rotationToObstacle = 0;
}

void followLine() {
    updateLineData();
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
    // rotate(90);
    // delay(1500);
    // rotate(-90);
    // delay(INFINITY);
}