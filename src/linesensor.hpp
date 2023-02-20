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

bool calibrateSensor() {
    return true;
    digitalWrite(LED_BUILTIN, HIGH); 
    qtr.setTypeAnalog();
    qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);
    digitalWrite(LED_BUILTIN, HIGH); 
    for (uint16_t i = 0; i < 400; i++){
        qtr.calibrate();
    }
    digitalWrite(LED_BUILTIN, LOW); 
    return false;
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
    while (calibrateSensor()){
        drive(100,100);
    }
    
}

void updateLineData() {
    position = qtr.readLineBlack(sensorValues);
    isOnLine = false;
    for (int i = 0; i < 8; i++) {
        if (sensorValues[i] > 975) {
            isOnLine = true;
            break;
        }
    }
}