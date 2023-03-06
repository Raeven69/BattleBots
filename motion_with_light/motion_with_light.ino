/************************
 ***      Motion      ***
 ************************/

//===[Library]========================================================
#include <Adafruit_NeoPixel.h>

//===[Constants]======================================================
const int LED_PIN = 12;              //NI <- Neopixel
const int LED_COUNT = 4;             //Amount Neopixels
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const int leftMotorForward = 5;      //B2 <- Motor
const int rightMotorForward = 9;     //A2 <- Motor
const int leftMotorBackward = 10;    //A1 <- Motor
const int rightMotorBackward = 6;    //B1 <- Motor

unsigned long timerNe = 0;           //start value for millis Neopixels

/************************
 ***       setup      ***
 ************************/

void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    strip.begin();                   // INITIALIZE NeoPixel strip object
    strip.show();                    // Turn OFF all pixels
    strip.setBrightness(100);        // Set BRIGHTNESS
    
  //===[I/O Constant]=================================================
  pinMode(LED_PIN, OUTPUT);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);
}

void loop() {
  //===[motion]=======================================================
  /*motionForward(150, 150);
  stationary();*/
  motionLeft(180, 0, 150);
  /*stationary();
  motionBackward(150, 150);
  stationary();
  motionRight(0, 180, 150);
  stationary();*/
}

/************************
 ***    functions     ***
 ************************/

//===[Motion Forward]=================================================
  //...
void motionForward(int left, int right){
    
  analogWrite(leftMotorForward, left);
  analogWrite(rightMotorForward, right);
  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW);
  
  strip.clear();
  strip.setPixelColor(2, strip.Color(255, 255, 255));
  strip.setPixelColor(3, strip.Color(255, 255, 255));  
  strip.show();
}

//===[Motion Backward]================================================
  //...
void motionBackward(int left, int right){
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
  analogWrite(leftMotorBackward, left);
  analogWrite(rightMotorBackward, right);

  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.show();
}

//===[Motion left]====================================================
  //When the robot turns left, the outer corner neopixels need to blink in white
void motionLeft(int left, int right, int blinkSpeed){
  digitalWrite(leftMotorForward, left);
  digitalWrite(rightMotorForward, right);
  analogWrite(leftMotorBackward, LOW);
  analogWrite(rightMotorBackward, LOW);

  strip.clear();
  if(millis() >= timerNe + blinkSpeed){
    timerNe = millis() + blinkSpeed;
    
    strip.setPixelColor(0, strip.Color(255, 255, 255));
    strip.setPixelColor(3, strip.Color(255, 255, 255));
  }
  strip.show();
}

//===[Motion right]===================================================
  //When the robot turns right, the outer corner neopixels need to blink in white
void motionRight(int left, int right, int blinkSpeed){
  digitalWrite(leftMotorForward, left);
  digitalWrite(rightMotorForward, right);
  analogWrite(leftMotorBackward, LOW);
  analogWrite(rightMotorBackward, LOW);

  strip.clear();
  strip.clear();
  if(millis() >= timerNe + blinkSpeed){
    timerNe = millis() + blinkSpeed;
    
    strip.setPixelColor(1, strip.Color(255, 255, 255));
    strip.setPixelColor(2, strip.Color(255, 255, 255));
  }
  strip.show();
}

//===[Motion stationary]==============================================
  //When the robot hit the brakes, the neopixels need to turn red
void stationary(){
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
  analogWrite(leftMotorBackward, LOW);
  analogWrite(rightMotorBackward, LOW);

  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.setPixelColor(2, strip.Color(0, 255, 0));
  strip.setPixelColor(3, strip.Color(0, 255, 0));
  strip.show();
}
