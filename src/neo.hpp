#include <Adafruit_NeoPixel.h>

#define LED_PIN   9
#define NUMPIXELS  4
Adafruit_NeoPixel neoLed(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setNeoLed(int green, int red, int blue) {
  for(int i=0; i<NUMPIXELS; i++) {
    neoLed.setPixelColor(i, neoLed.Color(green, red, blue));
    neoLed.show();
  }
}

void neoDrive() {
  setNeoLed(200,0,200);
}

void omegaPanic() {
  setNeoLed(0,255,0);
}

void neoRandom() {
  setNeoLed(random(200), random(200), random(200));
}

void neoCalibrate() {
  setNeoLed(13,106,173);
}

void neoLeft() {
  neoLed.setPixelColor(0, neoLed.Color(165, 255, 0));
  neoLed.setPixelColor(1, neoLed.Color(0,0,0));
  neoLed.setPixelColor(2, neoLed.Color(0,0,0));
  neoLed.setPixelColor(3, neoLed.Color(165, 255, 0));
  neoLed.show();
}

void neoRight() {
  neoLed.setPixelColor(0, neoLed.Color(0,0,0));
  neoLed.setPixelColor(1, neoLed.Color(165, 255, 0));
  neoLed.setPixelColor(2, neoLed.Color(165, 255, 0));
  neoLed.setPixelColor(3, neoLed.Color(0,0,0));
  neoLed.show();
}
