#include "lib/QTRSensors.hpp"

const int lineSensorOuterRight = A0;
const int lineSensorFarRight = A1;
const int lineSensorRight = A2;
const int lineSensorInnerRight = A3;
const int lineSensorInnerLeft = A4;
const int lineSensorLeft = A5;
const int lineSensorFarLeft = A6;
const int lineSensorOuterLeft = A7;

QTRSensors qtr;
uint16_t sensorValues[8];
bool isOnLine;
uint16_t position;

void calibrateSensor() {
    digitalWrite(LED_BUILTIN, HIGH);
    openGrapper(); 
    qtr.setTypeAnalog();
    qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);
    digitalWrite(LED_BUILTIN, HIGH);
    drive(255,255);
    delay(20);
    drive(86,72);
    for (uint16_t i = 0; i < 150; i++){
        qtr.calibrate();
    }
    drive(0,0);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    closeGrapper();
    rotate(-90);
}

void initLineSensor() {
    pinMode(lineSensorOuterRight, INPUT);
    pinMode(lineSensorFarRight, INPUT);
    pinMode(lineSensorRight, INPUT);
    pinMode(lineSensorInnerRight, INPUT);
    pinMode(lineSensorInnerLeft, INPUT);
    pinMode(lineSensorLeft, INPUT);
    pinMode(lineSensorFarLeft, INPUT);
    pinMode(lineSensorOuterLeft, INPUT);
    calibrateSensor();
    
}

void updateLineData() {
    position = qtr.readLineBlack(sensorValues);
    bool isCurrentlyOnLine = false;
    for (int i = 0; i < 8; i++) {
        if (sensorValues[i] > qtr.calibrationOn.maximum[i] - 25) {
            isCurrentlyOnLine = true;
            break;
        }
    }
    isOnLine = isCurrentlyOnLine;
}