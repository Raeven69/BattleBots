/************************************
***         Line sensor           ***
************************************/

// Include the QTRSensors library for calibrating the sensors
#include "lib/QTRSensors.hpp"

// The sensor pins
const int lineSensorOuterRight = A0;
const int lineSensorFarRight = A1;
const int lineSensorRight = A2;
const int lineSensorInnerRight = A3;
const int lineSensorInnerLeft = A4;
const int lineSensorLeft = A5;
const int lineSensorFarLeft = A6;
const int lineSensorOuterLeft = A7;

// The QTRSensors object for storing calibrations
QTRSensors qtr;
// An array to store the current values of the sensors in
uint16_t sensorValues[8];
// Variable to keep track of whether the robot is on the line or not
bool isOnLine;
// Variable to store the position of the line relative to the sensors
uint16_t position;
// Calibration line counting variables
bool wasOnLines = false;
// Variable for keeping the last time the robot was on a black line
long lastAllLines = 0;

// Function for updating the sensor data and checking whether the robot is on the line or not
void updateLineData()
{
    position = qtr.readLineBlack(sensorValues);
    bool isCurrentlyOnLine = false;
    for (int i = 0; i < 8; i++)
    {
        if (sensorValues[i] > qtr.calibrationOn.maximum[i] - 100)
        {
            isCurrentlyOnLine = true;
            break;
        }
    }
    isOnLine = isCurrentlyOnLine;
}

// Function for checking whether all sensors are seeing a black line
bool isAllOnLine()
{
    updateLineData();
    bool allSensors = true;
    for (int i = 0; i < 8; i++)
    {
        if (sensorValues[i] < qtr.calibrationOn.maximum[i] - 100)
        {
            allSensors = false;
            break;
        }
    }
    return allSensors;
}

// Function to calibrate the sensors
void calibrateSensor()
{
    // First open the grapper, then drive forward over the calibration lines, then proceed to pick up the pawn and rotate towards the course
    openGrapper(); 
    qtr.setTypeAnalog();
    qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);
    // drive(255, 255);
    // delay(30);
    // drive(90, 76);
    while (true)
    {
        qtr.calibrate();
        driveStraight();
        if (isAllOnLine())
        {
            if (wasOnLines && lastAllLines > millis() - 2250)
            {
                while (true)
                {
                    qtr.calibrate();
                    driveStraight();
                    if (!isAllOnLine())
                    {
                        delay(250);
                        break;
                    }
                }
                break;
            }
            else
            {
                wasOnLines = true;
                lastAllLines = millis();
            }
        }
        else
        {
            wasOnLines = false;
            lastAllLines = 0;
        }
    }
    resetDriveStraight();
    delay(100);
    closeGrapper();
    rotate(-90);
}

// Initialize the line sensor pins and start calibrating
void initLineSensor()
{
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