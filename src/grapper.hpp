/************************************
***            Grapper            ***
************************************/

// Include the servo library for controlling servo motors
#include "lib/Servo.hpp"

// The servo object to write values to
Servo grapper;
// Variable for storing whether the grapper is closed
bool grapperClosed = false;
// The pin for the grapper
const int grapperPin = 2;

// Attach the grapper pin to the servo object
void initGrapper()
{
    grapper.attach(grapperPin);
}

// Open the grapper
void openGrapper()
{
    grapper.write(90);
    grapperClosed = false;
}

// Close the grapper
void closeGrapper()
{
    grapper.write(45);
    grapperClosed = true;
}

void detachGrapper()
{
    grapper.detach();
}