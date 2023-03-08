#include <QTRSensors.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#define bluetoothRX 2
#define bluetoothTX 10
SoftwareSerial mySerial(bluetoothRX, bluetoothTX);

QTRSensors qtr;

const int leftMotorForward = 5; //5
const int leftMotorBackward = 6; //6
const int rightMotorForward = 3; //9
const int rightMotorBackward = 11; //10

const int leftRotationPin = 7;
const int rightRotationPin = 4;

const int lineSensorOuterRight = A0;
const int lineSensorFarRight = A1;
const int lineSensorRight = A2;
const int lineSensorInnerRight = A3;
const int lineSensorInnerLeft = A4;
const int lineSensorLeft = A5;
const int lineSensorFarLeft= A6;
const int lineSensorOuterLeft = A7;

const int ledPin = 12; //NI <- Neopixel
const int ledCount = 4; //Amount Neopixels
Adafruit_NeoPixel strip(ledCount, ledPin, NEO_GRB + NEO_KHZ800);

bool goesForward = false;
bool isDriving = false;

bool turning = false;

char val;

bool forward = true;

bool endReached = false;

bool outerRight;
bool farRight;
bool right;
bool innerRight;
bool innerLeft;
bool left;
bool farLeft;
bool outerLeft;

bool isTurnRight = false;
bool onceTurnRight = false;
bool isTurnLeft = false;
bool onceTurnLeft = false;

int period = 1900;
unsigned long time_now = 0;
unsigned long timeNe = 0; // Start value for millis Neopixel

int rotationState;
int rotationLastState;

int leftRotationState;
int leftRotationLastState;
int rightRotationState;
int rightRotationLastState;

bool leftRotating = false;
bool rotating = false;

int calibratedValue;

bool calibrating = true;

// Sensor Calibration
const int calibrationTime = 40; // in milliseconds * 20 (50 = 1 second)
const bool shouldCalibrate = true;

bool starting = true;

int getLineSensorSensitivity(int margin = 100)
{
    int totalSize = 0;
    for (int i = 0; i < 8; i++)
    {
        totalSize += qtr.calibrationOn.maximum[i];
    }
    return (int)(totalSize / 8 - margin);
}


void setup() {
  // put your setup code here, to run once:
  pinMode(lineSensorOuterRight, INPUT);
  pinMode(lineSensorFarRight, INPUT);
  pinMode(lineSensorRight, INPUT);
  pinMode(lineSensorInnerRight, INPUT);
  pinMode(lineSensorInnerLeft, INPUT);
  pinMode(lineSensorLeft, INPUT);
  pinMode(lineSensorFarLeft, INPUT);
  pinMode(lineSensorOuterLeft, INPUT);

  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

//===[Neopixels]========================================================
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    strip.begin();                   // INITIALIZE NeoPixel strip object
    strip.show();                    // Turn OFF all pixels
    strip.setBrightness(100);        // Set BRIGHTNESS
    
  pinMode(ledPin, OUTPUT);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Open serial communication for the Bluetooth Serial Port
  mySerial.begin(9600);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);
/*
 if (shouldCalibrate)
  {
    int i;
    Serial.println("");
    Serial.print("Calibrating");
    for (i = 0; i < calibrationTime; i++)
    {
      //drive(140,140);
      if (i % 10 == 0)
      {
        (i % 20 == 0) ? drive(-140,140) : drive(140,-140);
      }
      else if (i % 15 == 0)
      {
        (i % 30 == 0) ? drive(-140,140) : drive(140,-140); 
      }
      if ((i % 100 == 0 || i == 150) && i > 0)
      {
        Serial.print(".");
      }
      qtr.calibrate();
      delay(20);
    }
    Serial.println("");
    Serial.println("Calibration complete");
    drive(0,0);
    calibratedValue = getLineSensorSensitivity();
    Serial.println(calibratedValue);
  }
*/
}



void loop() {
  // put your main code here, to run repeatedly:
  bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
  bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
  bool right = analogRead(lineSensorRight) > calibratedValue;
  bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
  bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
  bool left = analogRead(lineSensorLeft) > calibratedValue;
  bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
  bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
  
  if (starting == true){
    start();
  }
  else if(endReached == false && isTurnRight == false && isTurnLeft == false && rotating == false && starting == false){
    followLine();
  }
  else if (rotating == true && starting == false){
    rotate(180);
    rotating = false;
  }
  else if (isTurnRight == true && onceTurnRight == false && starting == false){
    moveForward(170,0);
    delay(400);
    moveStop();
    onceTurnRight = true;
    mySerial.println("onceTurnRight = true");
  }
  else if (isTurnRight == true && onceTurnRight == true && starting == false){
    turnRight();
    mySerial.println("turnRight");
  }
  else if (isTurnLeft == true && onceTurnLeft == false && starting == false){
    moveForward(0,150);
    delay(400);
    moveStop();
    onceTurnLeft = true;
    mySerial.println("onceTurnLeft = true");
  }
  else if (isTurnLeft == true && onceTurnLeft == true && starting == false){
    mySerial.println("Turning left");
    turnLeft();
  }
  else{
    moveStop();
  }

}

void moveForward(int left, int right){

  goesForward = true;
  isDriving = true; 

  analogWrite(leftMotorForward, left);
  analogWrite(rightMotorForward, right);

  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW); 

  strip.clear();
  strip.setPixelColor(2, strip.Color(255, 255, 255));
  strip.setPixelColor(3, strip.Color(255, 255, 255));  
  strip.show();
}

void moveBackward(int left, int right){

  goesForward = false;
  isDriving = true;

  analogWrite(leftMotorBackward, left);
  analogWrite(rightMotorBackward, right);
  
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);

  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.show();
}

void moveStop(){

  isDriving = false;
  goesForward = false;
  
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
  digitalWrite(leftMotorBackward, LOW);

  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.setPixelColor(2, strip.Color(0, 255, 0));
  strip.setPixelColor(3, strip.Color(0, 255, 0));
  strip.show();
}

void turnRight(){
  bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
  bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
  bool right = analogRead(lineSensorRight) > calibratedValue;
  bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
  bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
  bool left = analogRead(lineSensorLeft) > calibratedValue;
  bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
  bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
  if(innerRight == 0 || innerLeft == 0){
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
    moveForward(170,0);
  }
  if (innerLeft == 1 || (innerRight == 1 && innerLeft == 1)){
    moveStop();
    isTurnRight = false;
    onceTurnRight = false;
  }
  
  strip.clear();
  strip.clear();
  if(millis() >= timeNe + 150){
    timeNe = millis() + 150;
    
    strip.setPixelColor(1, strip.Color(255, 255, 255));
    strip.setPixelColor(2, strip.Color(255, 255, 255));
  }
  strip.show();
}

void turnLeft(){
  bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
  bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
  bool right = analogRead(lineSensorRight) > calibratedValue;
  bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
  bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
  bool left = analogRead(lineSensorLeft) > calibratedValue;
  bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
  bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
  if(innerRight == 0 || innerLeft == 0){
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
    moveForward(0,170);
  }
  if (innerRight == 1 || (innerRight == 1 && innerLeft == 1) || innerLeft == 1){
    moveStop();
    isTurnLeft = false;
    onceTurnLeft = false;
  }

  strip.clear();
  if(millis() >= timeNe + 150){
    timeNe = millis() + 150;
    
    strip.setPixelColor(0, strip.Color(255, 255, 255));
    strip.setPixelColor(3, strip.Color(255, 255, 255));
  }
  strip.show();
}

void followLine(){
  bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
  bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
  bool right = analogRead(lineSensorRight) > calibratedValue;
  bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
  bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
  bool left = analogRead(lineSensorLeft) > calibratedValue;
  bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
  bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;

  if( mySerial.available() > 0){
    val = mySerial.read();
    mySerial.println(val);
    if( val == 'o'){
      endReached = true;
    }
  }
  
  if (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1 && innerRight == 1 && right == 1 && farRight == 1){
    moveForward(110,190);
    mySerial.println("Naar links");
  }
  else if (((outerRight == 1 && farLeft == 0) && (innerLeft == 1 || innerRight == 1)) || (outerRight == 1 && left == 1 && farLeft == 0 && outerLeft == 0) || (left == 1 && farRight == 1 && farLeft == 0) || (right == 1 && farRight == 1 && outerRight == 1 && farLeft == 0) || (innerRight == 1 && right == 1 && farRight == 1 && outerRight == 1 && farLeft == 0) || (left == 1 && innerLeft == 1 && outerRight == 1 && farLeft == 0) || (left == 1 && farRight == 1 && farLeft == 0) || (farRight == 1 && outerRight == 1 && farLeft == 0) || (outerRight == 1 && (innerRight == 1 || innerLeft == 1 || left == 1)  && farLeft == 0)) {
    mySerial.println("Bocht naar rechts");
    moveStop();
    isTurnRight = true;
  }
  else if (innerLeft == 1 && innerRight == 1){
    moveForward(200,200);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (innerLeft == 1 && innerRight == 0){
    moveForward(180,200);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (innerLeft == 0 && innerRight == 1){
    moveForward(200,180);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (left == 1){
    moveForward(135,200);
    mySerial.println("Iets naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (right == 1){
    moveForward(200,135);
    mySerial.println("Iets naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (farLeft == 1){
    moveForward(0,180);
    mySerial.println("Naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (farRight == 1){
    moveForward(180,0);
    mySerial.println("Naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (outerRight == 1){
    moveForward(180,0);
    mySerial.println("Naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (outerLeft == 1){
    moveForward(0,180);
    mySerial.println("Naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else{
    mySerial.println("Ik zie niks meer");
    if (endReached == true){
      moveStop();
    }
    else{
      drive(135,135);
      delay(300);
      drive(0,0);
      delay(150);
      rotate(90);
      delay(200);
      bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
      bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
      bool right = analogRead(lineSensorRight) > calibratedValue;
      bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
      bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
      bool left = analogRead(lineSensorLeft) > calibratedValue;
      bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
      bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
      if(outerRight == 0 && farRight == 0 && right == 0 && innerRight == 0 && innerLeft == 0 && left == 0 && farRight == 0 && outerRight == 0){
        rotate(-180);  
        delay(200);
        bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
        bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
        bool right = analogRead(lineSensorRight) > calibratedValue;
        bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
        bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
        bool left = analogRead(lineSensorLeft) > calibratedValue;
        bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
        bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
        if(outerRight == 0 && farRight == 0 && right == 0 && innerRight == 0 && innerLeft == 0 && left == 0 && farRight == 0 && outerRight == 0){
          rotate(-93);
          delay(200);
        }
        else{
          return;
        }
      }
      else{
        return;
      }
    }
  }
}

void rotate(int rotation) {
    int counter = 0;
    while (counter < (int)((135.0/360.0) * abs(rotation))) { 
        if (rotation > 0) {
            drive(160, -160);
        } else {
            drive(-160, 160);
        }
        leftRotationState = digitalRead(leftRotationPin);
        rightRotationState = digitalRead(rightRotationPin);
        if (leftRotationState != leftRotationLastState || rightRotationState != rightRotationLastState) {
            counter++;
        }
        leftRotationLastState = leftRotationState;
        rightRotationLastState = rightRotationState;
    }
    drive(0, 0);
}


void drive(int left, int right) {
    if (left < 0) {
        analogWrite(leftMotorForward, 0);
        analogWrite(leftMotorBackward, abs(left));
    } else {
        analogWrite(leftMotorForward, left);
        analogWrite(leftMotorBackward, 0);
    }
    if (right < 0) {
        analogWrite(rightMotorForward, 0);
        analogWrite(rightMotorBackward, abs(right));
    } else {
        analogWrite(rightMotorForward, right);
        analogWrite(rightMotorBackward, 0);
    }
    isDriving = abs(left) + abs(right) > 0;
}

void start(){
  if (shouldCalibrate)
  {
    int i;
    for (i = 0; i < calibrationTime; i++)
    {
      drive(200,200);
      delay(10);
      drive(99,75);
      qtr.calibrate();
      delay(20);
    }
    Serial.println("");
    Serial.println("Calibration complete");
    calibratedValue = getLineSensorSensitivity();
    Serial.println(calibratedValue);
  }
  if(outerLeft == 0 && farLeft == 0 && left == 0 && innerLeft == 0 && innerRight == 0 && right == 0 && farRight == 0 && outerRight == 0){ // Einde zwarte vlak is bereikt
    drive(0,0);
    delay(300);
    // Close gripper
    rotate(-92);
    drive(155,130);
    starting = false;
  }
  
}
