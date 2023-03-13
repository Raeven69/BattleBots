// Include the library for controlling the NeoPixel leds
#include "lib/Adafruit_NeoPixel.hpp"

/************************************
***           Neo Pixels          ***
************************************/

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

// Function for flashing lights after finishing the course
void neoFinish()
{
    // Run forever
    while (true)
    {
        // Loop over r, g and b values
        int rgb[3] = {0, 0, 0};
        for (int color = 0; color < 3; color++)
        {
            // Increment the value for each color by 1
            for (int i = 0; i < 255; i++)
            {
                rgb[color]++;
                uint32_t ledColor = neoLed.Color(rgb[0], rgb[1], rgb[2]);
                neoLed.setPixelColor(0, ledColor);
                neoLed.setPixelColor(1, ledColor);
                neoLed.setPixelColor(2, ledColor);
                neoLed.setPixelColor(3, ledColor);
                neoLed.show();
            }
        }
        // Do the same but by deincrementing
        for (int color = 0; color < 3; color++)
        {
            for (int i = 255; i > 0; i--)
            {
                rgb[color]--;
                uint32_t ledColor = neoLed.Color(rgb[0], rgb[1], rgb[2]);
                neoLed.setPixelColor(0, ledColor);
                neoLed.setPixelColor(1, ledColor);
                neoLed.setPixelColor(2, ledColor);
                neoLed.setPixelColor(3, ledColor);
                neoLed.show();
            }
        }
    }
}