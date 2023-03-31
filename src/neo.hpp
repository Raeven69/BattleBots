// Include the library for controlling the NeoPixel leds
#include "lib/Adafruit_NeoPixel.hpp"

/************************************
***           Neo Pixels          ***
************************************/

// Define variables for the NeoPixel pin, amount of leds and the neoLed object
int neoLedPin = 13;
int neoLedCount = 4;
Adafruit_NeoPixel neoLed(neoLedCount, neoLedPin, NEO_RGB + NEO_KHZ800);

// Initialize the NeoPixel leds
void initNeoPixels(){
    neoLed.begin();
}

void neoDrive(int left, int right){
    uint32_t leftColor, rightColor;
    // Bot is going in a straight line
    if (left == right) {  
        neoLed.clear();
        neoLed.setPixelColor(2, neoLed.Color(255, 255, 255));
        neoLed.setPixelColor(3, neoLed.Color(255, 255, 255)); 
    } 
    else if (left > right) {  
        // Battlebot is turning left
        neoLed.clear();
        neoLed.setPixelColor(1, neoLed.Color(255, 165, 0));
        neoLed.setPixelColor(2, neoLed.Color(255, 165, 0)); 
      
    }
    else {  
        // Battlebot is turning right
        neoLed.clear();
        neoLed.setPixelColor(0, neoLed.Color(255, 165, 0));
        neoLed.setPixelColor(3, neoLed.Color(255, 165, 0));
    }

    neoLed.show();
}


// Function for flashing lights after finishing the course
void neoFinish(){
    // Run forever
    while (true){
        // Loop over r, g and b values
        int rgb[3] = {0, 0, 0};
        for (int color = 0; color < 3; color++){
            // Increment the value for each color by 1
            for (int i = 0; i < 255; i++){
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
        for (int color = 0; color < 3; color++){
            for (int i = 255; i > 0; i--){
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