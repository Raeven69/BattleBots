#include <Adafruit_NeoPixel.h>


const int neoPixelPin = 13;
const int numPixels = 4;

Adafruit_NeoPixel neoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

void setup(){
  neoPixel.begin();
  neoPixel.setBrightness(50);
}

void loop(){
  neoPixel.setPixelColor(0, neoPixel.Color(0, 255, 0));
  neoPixel.setPixelColor(1, neoPixel.Color(0, 255, 0));
  neoPixel.setPixelColor(2, neoPixel.Color(255, 255, 255));
  neoPixel.setPixelColor(3, neoPixel.Color(255, 255, 255));
  neoPixel.show();
}

void backward(){
  neoPixel.setPixelColor(0, neoPixel.Color(0, 255, 0));
  neoPixel.setPixelColor(1, neoPixel.Color(0, 255, 0));
  neoPixel.show();
  delay(500);
  neoPixel.clear();
  neoPixel.show();
  delay(500);
}

void right(){
  neoPixel.setPixelColor(1, neoPixel.Color(75, 255, 0));
  neoPixel.setPixelColor(2, neoPixel.Color(75, 255, 0));
  neoPixel.show();
  delay(500);
  neoPixel.clear();
  neoPixel.show();
  delay(500);
}

void left(){
  neoPixel.setPixelColor(0, neoPixel.Color(75, 255, 0));
  neoPixel.setPixelColor(3, neoPixel.Color(75, 255, 0));
  neoPixel.show();
  delay(500);
  neoPixel.clear();
  neoPixel.show();
  delay(500);
}

void forward(){
  neoPixel.setPixelColor(0, neoPixel.Color(0, 255, 0));
  neoPixel.setPixelColor(1, neoPixel.Color(0, 255, 0));
  neoPixel.setPixelColor(2, neoPixel.Color(255, 255, 255));
  neoPixel.setPixelColor(3, neoPixel.Color(255, 255, 255));
  neoPixel.show();
}
