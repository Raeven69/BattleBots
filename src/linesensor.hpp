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

void initLineSensor() {
    pinMode(lineSensorOuterRight, INPUT);
    pinMode(lineSensorFarRight, INPUT);
    pinMode(lineSensorRight, INPUT);
    pinMode(lineSensorInnerRight, INPUT);
    pinMode(lineSensorInnerLeft, INPUT);
    pinMode(lineSensorLeft, INPUT);
    pinMode(lineSensorFarLeft, INPUT);
    pinMode(lineSensorOuterLeft, INPUT);
    qtr.setTypeAnalog();
    qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);
    qtr.calibrate();
    qtr.calibrationOn.minimum[0] = 487;
    qtr.calibrationOn.minimum[1] = 416;
    qtr.calibrationOn.minimum[2] = 398;
    qtr.calibrationOn.minimum[3] = 447;
    qtr.calibrationOn.minimum[4] = 407;
    qtr.calibrationOn.minimum[5] = 453;
    qtr.calibrationOn.minimum[6] = 538;
    qtr.calibrationOn.minimum[7] = 556;
    qtr.calibrationOn.maximum[0] = 990;
    qtr.calibrationOn.maximum[1] = 977;
    qtr.calibrationOn.maximum[2] = 971;
    qtr.calibrationOn.maximum[3] = 977;
    qtr.calibrationOn.maximum[4] = 977;
    qtr.calibrationOn.maximum[5] = 979;
    qtr.calibrationOn.maximum[6] = 988;
    qtr.calibrationOn.maximum[7] = 994;
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