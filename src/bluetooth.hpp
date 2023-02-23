#include "lib/SoftwareSerial.hpp"

const int receivePin = 13;
const int transmitPin = 8;

SoftwareSerial bluetooth(receivePin, transmitPin);

void initBluetooth()
{
    pinMode(receivePin, OUTPUT);
    pinMode(transmitPin, INPUT);
    Serial.begin(9600);
    bluetooth.begin(9600);
}

void communicate()
{
    if (Serial.available() > 0)
    {
        bluetooth.write(Serial.read());
    }
    if (bluetooth.available() > 0)
    {
        Serial.write(bluetooth.read());
    }
}