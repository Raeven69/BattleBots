/************************************
***           Bluetooth           ***
************************************/

// Include the SoftwareSeriar library for communication on other digital pins.
#include "lib/SoftwareSerial.hpp"

// I/O pins for receiving/transmitting
const int receivePin = 13;
const int transmitPin = 8;

SoftwareSerial bluetooth(receivePin, transmitPin);

// Initialize the pins for bluetooth communication 
void initBluetooth()
{
    pinMode(receivePin, OUTPUT);
    pinMode(transmitPin, INPUT);
    // Set the baud rate for the Serial port
    Serial.begin(9600);
    // Set the baud rate for the SerialSoftware object
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