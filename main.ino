// Import the code for different arduino modules
#include "src/engine.hpp"
#include "src/sonar.hpp"
#include "src/grapper.hpp"
#include "src/linesensor.hpp"
#include "src/bluetooth.hpp"

// Line direction variable for storing the location of the line incase the robot loses it
char* lineDirection = "none";
// Variable for storing the next time the line direction can reset
long nextLineDetection = millis();
// Variable for storing the last time all sensors were detecting a line
long lastAllOnLine = 0;
// Variable that stores whether the last time all sensors were checked they were on all lines
bool wasAllOnLine = false;
// Variable for storing whether the robot has finished the course
bool isFinished = false;

// Function for positioning the pawn at the end of the course
void positionPawn()
{
    drive(0, 0);
    delay(200);
    while (isAllOnLine())
    {
        drive(-150, -150);
    }
    drive(0, 0);
    delay(200);
    while (!isAllOnLine())
    {
        drive(150, 150);
    }
    drive(0, 0);
    delay(200);
    // Open the grapper and drive backwards for 1 second
    openGrapper();
    delay(200);
    drive(-255, -255);
    delay(1000);
    drive(0, 0);
}

// Function for avoiding obstacles
void avoidObstacle()
{
    rotate(60);
    drive(255, 255);
    delay(300);
    drive(100, 255);
    delay(500);
    updateLineData();
    while (!isOnLine)
    {
        updateLineData();
    }
}

// Function for checking whether the robot has finished the course
void checkFinished()
{
    if (isAllOnLine()) {
        // If the robot already was on all lines and more than half a second has passed, the robot is at the end of the course and thus finished
        if (wasAllOnLine && lastAllOnLine > millis() - 500)
        {
            isFinished = true;
        }
        // If it wasnt yet set the values for the next time we check whether the robot is finished
        else if (!wasAllOnLine)
        {
            wasAllOnLine = true;
            lastAllOnLine = millis();
        }
    }
    // Reset the values incase the robot was on all black but no longer is, without half a second passing
    else
    {
        wasAllOnLine = false;
        lastAllOnLine = 0;
    }
}

// Function for following the black line
void followLine()
{
    // First update the sensor data
    updateLineData();
    if (isOnLine)
    {
        // Calculate and set the speed of the left & right wheel based on how far the line is from the middle sensors
        int leftSpeed = (position < 2500) ? min(max((int)(255 * (3500 - position) / 3500), 50), 255) : 255;
        int rightSpeed = (position > 4500) ? min(max((int)(255 * (position - 3500) / 3500), 50), 255) : 255;
        drive(leftSpeed, rightSpeed);
        // If the line is on the outer left of the robot, set the line direction memory to the left
        if (position < 1000)
        {
            lineDirection = "left";
            nextLineDetection = millis() + 1000;
        }
        // If the line is on the outer right of the robot, set the line direction memory to the right
        else if (position > 6000)
        {
            lineDirection = "right";
            nextLineDetection = millis() + 1000;
        }
        // If enough time has passed to clear the line direction memory, proceed to do so
        else if (nextLineDetection < millis())
        {
            lineDirection = "none";
        }
    }
    // If the line is no longer found, but according to the memory it is on the left, steer to the left and set NeoPixels accordingly
    else if (lineDirection == "left")
    {
        drive(50, 255);
        nextLineDetection = millis() + 1000;
    }
    // If it's supposedly on the right, steer to the right
    else if (lineDirection == "right")
    {
        drive(255, 50);
        nextLineDetection = millis() + 1000;
    }
}

void setup()
{
    // Initialize the different modules
    initEngine();
    initGrapper();
    initLineSensor();
    initSonar();
    initBluetooth();
}

void loop()
{
    if (!isFinished)
    {
        // If the robot is not finished but needs to start driving, proceed to do so
        if (!isDriving)
        {
            drive(255, 255);
        }
        // If an object is found in front of the robot, avoid it
        if (isBlocked())
        {
            avoidObstacle();
        }
        // Otherwise try to follow the line while making sure the robot has not finished yet
        else
        {
            checkFinished();
            followLine();
        }
    }
    // If the robot is finished, position the pawn on the black square, and flash the NeoPixel leds for eternity
    else
    {
        positionPawn();
        neoFinish();
    }
}