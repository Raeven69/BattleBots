/************************************
***           INF1A               ***
***         13-3-2023             ***  
***          BeunBot              *** 
***         Versie 1.0            ***
************************************/

// Import the code for different arduino modules
#include "src/engine.hpp"
#include "src/sonar.hpp"
#include "src/grapper.hpp"
#include "src/linesensor.hpp"

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
// Variable for keeping track of when the bot started it's journey
long startTime = millis();

// Function for positioning the pawn at the end of the course
void positionPawn()
{
    // Open the grapper and drive backwards for 1 second
    drive(0, 0);
    delay(200);
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
    delay(1000);
    drive(255, 255);
    updateLineData();
    while (!isOnLine)
    {
        updateLineData();
    }
}

// Function for checking whether the robot has finished the course
void checkFinished()
{
    if (bool currentAllOnline = isAllOnLine()) {
        // If the robot already was on all lines and more than half a second has passed, the robot is at the end of the course and thus finished
        if (wasAllOnLine && lastAllOnLine > millis() - 1000)
        {
            isFinished = true;
        }
        // If it wasnt yet set the values for the next time we check whether the robot is finished
        else
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
        const int minSpeed = 60;
        const int maxSpeed = 240;

        int leftSpeed, rightSpeed;

        if (position < 2500) {
            leftSpeed = (255 * (3500 - position)) / 3500;
            leftSpeed = max(minSpeed, leftSpeed);
            leftSpeed = min(maxSpeed, leftSpeed);
            rightSpeed = maxSpeed;
        }
        else if (position > 4500) {
            rightSpeed = (255 * (position - 3500)) / 3500;
            rightSpeed = max(minSpeed, rightSpeed);
            rightSpeed = min(maxSpeed, rightSpeed);
            leftSpeed = maxSpeed;
        }
        else {
            leftSpeed = maxSpeed;
            rightSpeed = maxSpeed;
        }

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
    initSonar();
    initLineSensor();
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
            followLine();
            if (millis() > startTime + 10000)
            {
                checkFinished();
            }
        }
    }
    // If the robot is finished, position the pawn on the black square, and flash the NeoPixel leds for eternity
    else
    {
        positionPawn();
        detachGrapper();
        neoFinish();
    }
}