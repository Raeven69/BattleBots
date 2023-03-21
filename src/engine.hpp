/************************************
***            Engine             ***
************************************/

// Inclde code for controlling the NeoPixel leds
#include "neo.hpp"

// I/O pins for the engine
const int leftMotorPin1 = 5;
const int leftMotorPin2 = 6;
const int rightMotorPin2 = 3;
const int rightMotorPin1 = 11;
const int leftRotationPin = 12;
const int rightRotationPin = 10;

// Pulse counters for counting rotation
int leftRotationState;
int leftRotationLastState;
int rightRotationState;
int rightRotationLastState;

// Variables for driving straight by counting pulses of the rotation sensor
int rightCounter = 0;
int rightState;
int rightLastState;
int leftCounter = 0;
int leftState;
int leftLastState;
long driveStart = 0;

// Variable to indicate whether the robot is driving or not
bool isDriving = false;

// Initialize the engine pins and NeoPixels
void initEngine()
{
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);
    pinMode(leftRotationPin, INPUT);
    pinMode(rightRotationPin, INPUT);
    initNeoPixels();
}

// Function to set the engine speeds of the robot
void drive(int left, int right)
{
    // If the left speed is negative, set the left wheel to go backward
    if (left < 0)
    {
        analogWrite(leftMotorPin1, 0);
        analogWrite(leftMotorPin2, abs(left));
    }
    // Otherwise go forwards
    else
    {
        analogWrite(leftMotorPin1, left);
        analogWrite(leftMotorPin2, 0);
    }
    // If the right speed is negative, set the right wheel to go backward
    if (right < 0)
    {
        analogWrite(rightMotorPin1, 0);
        analogWrite(rightMotorPin2, abs(right));
    }
    // Otherwise go forwards
    else
    {
        analogWrite(rightMotorPin1, right);
        analogWrite(rightMotorPin2, 0);
    }
    // Update the NeoPixels according to the speed
    neoDrive(left, right);
    // Update the variable to keep track of whether the robot is moving
    isDriving = abs(left) + abs(right) > 0;
}

void driveStraight()
{
    leftState = digitalRead(leftRotationPin);
    rightState = digitalRead(rightRotationPin);
    if (leftState != leftLastState)
    {
        leftLastState = leftState;
        leftCounter++;
    }
    if (rightState != rightLastState)
    {
        rightLastState = rightState;
        rightCounter++;
    }
    if (driveStart == 0)
    {
        driveStart = millis() + 10;
    }
    if (leftCounter > rightCounter)
    {
        if (millis() > driveStart)
        {
            drive(70, 90);
        }
        else
        {
            drive(230, 250);
        }
    }
    else
    {
        if (millis() > driveStart)
        {
            drive(90, 70);
        }
        else
        {
            drive(250, 230);
        }
    }
}

void resetDriveStraight()
{
    rightState = 0;
    leftState = 0;
    rightLastState = 0;
    leftLastState = 0;
    rightCounter = 0;
    leftCounter = 0;
    driveStart = 0;
    drive(0, 0);
}

// Function to rotate the robot a certain amount of degrees
void rotate(int rotation)
{
    int counter = 0;
    // Calculate the amount of pulses for the rotation sensor based on degrees
    int pulses = (int)((135.0/360.0) * abs(rotation));
    while (counter < pulses)
    {
        // If the rotation is positive, rotate to the right, otherwise rotate to the left
        if (rotation > 0)
        {
            drive(150, -150);
        }
        else
        {
            drive(-150, 150);
        }
        // Update the rotation states
        leftRotationState = digitalRead(leftRotationPin);
        rightRotationState = digitalRead(rightRotationPin);
        if (leftRotationState != leftRotationLastState || rightRotationState != rightRotationLastState)
        {
            counter++;
        }
        leftRotationLastState = leftRotationState;
        rightRotationLastState = rightRotationState;
    }
    // Stop rotating when the amount of degrees to rotate has been reached
    drive(0, 0);
    // Also reset the NeoPixels
    neoDrive(0, 0);
}