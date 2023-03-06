// Include the library for controlling the NeoPixel leds
#include "lib/Adafruit_NeoPixel.hpp"

// Define variables for the NeoPixel pin, amount of leds and the neoLed object
int neoLedPin = 9;
int neoLedCount = 4;
Adafruit_NeoPixel neoLed(neoLedCount, neoLedPin, NEO_RGB + NEO_KHZ800);

// Initialize the NeoPixel leds
void initNeoPixels()
{
    neoLed.begin();
}

// Function for setting the led colors according to the speed of the left and right wheel
void neoDrive(int left, int right)
{
    // Calculate the color for the left and right leds according to the speed
    uint32_t leftColor = neoLed.Color(left < 0 ? 255 : 255 - left, left < 0 ? 255 + left : 255, 0);
    uint32_t rightColor = neoLed.Color(right < 0 ? 255 : 255 - right, right < 0 ? 255 + right : 255, 0);
    // Apply the colors
    neoLed.setPixelColor(0, leftColor);
    neoLed.setPixelColor(1, rightColor);
    neoLed.setPixelColor(2, rightColor);
    neoLed.setPixelColor(3, leftColor);
    neoLed.show();
}