#include "src/engine.hpp"
#include "src/sonar.hpp"
#include "src/grapper.hpp"
#include "src/linesensor.hpp"

char* lineDirection = "none";
long nextLineDetection = millis();

void avoidObstacle()
{
    drive(255, 100);
    delay(750);
    drive(100, 255);
    delay(500);
    updateLineData();
    while (!isOnLine)
    {
        updateLineData();
    }
}

void followLine()
{
    updateLineData();
    if (isOnLine)
    {
        drive((position < 2500) ? min(max((int)(255 * (3500 - position) / 3500), 50), 255) : 255, (position > 4500) ? min(max((int)(255 * (position - 3500) / 3500), 50), 255) : 255);
        if (position < 1000)
        {
            lineDirection = "left";
            nextLineDetection = millis() + 1000;
        }
        else if (position > 6000)
        {
            lineDirection = "right";
            nextLineDetection = millis() + 1000;
        }
        else if (nextLineDetection < millis())
        {
            lineDirection = "none";
        }
    }
    else if (lineDirection == "left")
    {
        drive(50, 255);
        nextLineDetection = millis() + 1000;
    }
    else if (lineDirection == "right")
    {
        drive(255, 50);
        nextLineDetection = millis() + 1000;
    }
}

void setup()
{
    initEngine();
    initGrapper();
    initLineSensor();
    initSonar();
}

void loop()
{
    if (!grapperClosed)
    {
        closeGrapper();
    }
    if (!isDriving)
    {
        drive(255, 255);
    }
    if (isBlocked())
    {
        avoidObstacle();
    }
    else
    {
        followLine();
    }
}