/************************************
***     INF1A Bot Eugene Krabs    ***
***            13-3-2023          ***
***            Versie 0.8         ***
************************************/

/************************************
***           Libraries           ***
************************************/
#include <QTRSensors.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>                            // Library used to control the NeoPixels
#define bluetoothRX 2                                     // Define bluetoothRX pin 2
#define bluetoothTX 8                                     // Define bluetoothTX pin 8
SoftwareSerial mySerial(bluetoothRX, bluetoothTX);      

/************************************
***           Variables           ***
************************************/

const int gripperPin = 10;                                // Set gripperPin to pin 10

QTRSensors qtr;

//==[Const int motorpins]===================================
const int leftMotorForward = 5;                           // 5
const int leftMotorBackward = 6;                          // 6
const int rightMotorForward = 3;                          // 9
const int rightMotorBackward = 11;                        // 10

//===[Const int rotationpins]===============================
const int leftRotationPin = 7;                            // 
const int rightRotationPin = 4;                           // 

//===[Const int LineSenorpins]==============================
const int lineSensorOuterRight = A0;                      // 
const int lineSensorFarRight = A1;                        // 
const int lineSensorRight = A2;                           // 
const int lineSensorInnerRight = A3;                      // 
const int lineSensorInnerLeft = A4;                       // 
const int lineSensorLeft = A5;                            // 
const int lineSensorFarLeft= A6;                          // 
const int lineSensorOuterLeft = A7;                       // 

//===[Const int NeoPixels]==================================
const int ledPin = 12;                                    // set Neopixel pin to 12
const int ledCount = 4;                                   // Amount of Neopixels
Adafruit_NeoPixel strip(ledCount, ledPin, NEO_GRB + NEO_KHZ800);

//==[Set varibials]=========================================
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

//int period = 1900;
unsigned long timeNe = 0;                                 // Start value for millis Neopixel

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

bool gripperClosed = false;

bool moving = false;

//===[Sensor Calibration]===================================
const int calibrationTime = 35;                           // in milliseconds * 20 (50 = 1 second)
const bool shouldCalibrate = true;                        // Start calibrate

bool starting = true;

int getLineSensorSensitivity(int margin = 100){           // Get Sensitivity from lineSensor
    int totalSize = 0;
    for (int i = 0; i < 8; i++){
        totalSize += qtr.calibrationOn.maximum[i];
    }
    return (int)(totalSize / 8 - margin);
}

/************************************
***       Setup Digital I/O       ***
************************************/

void setup(){
  // put your setup code here, to run once:

  //===[Set lineSensor to INPUT]===========================
  pinMode(lineSensorOuterRight, INPUT);
  pinMode(lineSensorFarRight, INPUT);
  pinMode(lineSensorRight, INPUT);
  pinMode(lineSensorInnerRight, INPUT);
  pinMode(lineSensorInnerLeft, INPUT);
  pinMode(lineSensorLeft, INPUT);
  pinMode(lineSensorFarLeft, INPUT);
  pinMode(lineSensorOuterLeft, INPUT);

  //===[Set motor to OUTPUT]===============================
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  //===[Set gripperPin to OUTPUT]==========================
  pinMode(gripperPin, OUTPUT);

  //===[Set NeoPixel to OUTPUT]============================
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    strip.begin();                                        // INITIALIZE NeoPixel strip object
    strip.show();                                         // Turn OFF all pixels
    strip.setBrightness(100);                             // Set BRIGHTNESS

  pinMode(ledPin, OUTPUT);                                // Set ledPin to OUTPUT

  //===[Open serial communication for the Bluetooth Serial Port]
  Serial.begin(9600);                                     // Begin Serial
  while (!Serial){                                        // Wait till serial started
    ;
  }

  mySerial.begin(9600);

  qtr.setTypeAnalog();                                    // Initialize linesSensor
  qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);  // Initialize linesSensor
}

/************************************
***              Loop             ***
************************************/

void loop(){
  // Put your main code here, to run repeatedly:
  // Read lineSensor
  bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;   
  bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
  bool right = analogRead(lineSensorRight) > calibratedValue;
  bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
  bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
  bool left = analogRead(lineSensorLeft) > calibratedValue;
  bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
  bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
  
  if (starting == true){                                  // Start procedure
    openGripper();                                        // Open Gripper
    start();                        
  }
  else if(endReached == false && isTurnRight == false && isTurnLeft == false && rotating == false && starting == false){
    forwardLight();                                       // Turn on headlights
    followLine();                                         // FollowLine
  }
  else if(rotating == true && starting == false && endReached == false){
    rotateLight();                                        // Turnsignal
    rotate(180);                                          // Rotate 180 degrees
    rotating = false;                                     // Stop rotating
  }
  else if(isTurnRight == true && onceTurnRight == false && starting == false && endReached == false){
    rightLight();                                         // Turnsignal
    moveForward(170,0);                                   // Turn right
    delay(400);                                           // Delay
    moveStop();                                           // Stop turning
    onceTurnRight = true;                                 // It Moved away from the line
    mySerial.println("onceTurnRight = true");
  }
  else if(isTurnRight == true && onceTurnRight == true && starting == false && endReached == false){
    rightLight();                                         // Turn signal
    turnRight();                                          // Turn right
    mySerial.println("turnRight");
  }
  else if(isTurnLeft == true && onceTurnLeft == false && starting == false && endReached == false){
    moveForward(0,170);                                   // Turn left
    delay(350);                                           // Delay
    moveStop();                                           // Stop turning
    onceTurnLeft = true;                                  // It Moved away from the line
    mySerial.println("onceTurnLeft = true");
  }
  else if(isTurnLeft == true && onceTurnLeft == true && starting == false && endReached == false){
    mySerial.println("Turning left");
    turnLeft();                                           // Turn left
  }
  else{
    moveStop();                                           // Stop at the end
  }
}

/************************************
***            Functions          ***
************************************/

//===[NeoPixel]============================================
void forwardLight(){
  strip.clear();
  strip.setPixelColor(2, strip.Color(255, 255, 255));
  strip.setPixelColor(3, strip.Color(255, 255, 255));  
  strip.show();
}
  
void rotateLight(){
  strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.setPixelColor(2, strip.Color(0, 255, 0));
  strip.setPixelColor(3, strip.Color(0, 255, 0));
  strip.show();
}

void leftLight(){
  strip.clear();
  strip.clear();
  if(millis() >= timeNe + 150){
    timeNe = millis() + 150;
    
    strip.setPixelColor(0, strip.Color(255, 255, 50));
    strip.setPixelColor(3, strip.Color(255, 255, 50));
  }
  strip.show();
}

void rightLight(){
  strip.clear();
  if(millis() >= timeNe + 150){
    timeNe = millis() + 150;
    
    strip.setPixelColor(1, strip.Color(255, 255, 50));
    strip.setPixelColor(2, strip.Color(255, 255, 50));
    }
  strip.show();
}

//===[Elektromotoren + Line sensor]========================
void moveForward(int left, int right){

  goesForward = true;
  isDriving = true; 

  analogWrite(leftMotorForward, left);
  analogWrite(rightMotorForward, right);

  digitalWrite(leftMotorBackward, LOW);
  digitalWrite(rightMotorBackward, LOW); 
}

void moveBackward(int left, int right){    

  goesForward = false;
  isDriving = true;

  analogWrite(leftMotorBackward, left);
  analogWrite(rightMotorBackward, right);
  
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorForward, LOW);
}

void moveStop(){

  isDriving = false;
  goesForward = false;
  moving = false;
  
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
  digitalWrite(leftMotorBackward, LOW);
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
    moveForward(190,0);
  }
  if(innerLeft == 1 || (innerRight == 1 && innerLeft == 1) || innerRight == 1){    
                                                          // if line is in the middle
    moveStop();                                           // Stop
    isTurnRight = false;                                  // Stop turning
    onceTurnRight = false;      
  }
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
    moveForward(0,190);
  }
  if(innerRight == 1 || (innerRight == 1 && innerLeft == 1) || innerLeft == 1){    
                                                           // if line is in the middle
    moveStop();                                            // Stop
    isTurnLeft = false;                                    // Stop turning
    onceTurnLeft = false;
  }
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
  if(right == 1 && innerRight == 1 && innerLeft == 1 && left == 1 && farLeft == 1 && outerLeft == 1){
    drive(0,0);                                          // Stop
    delay(100);                                          // Delay
    drive(150,135);
    delay(200);
    drive(0,0);
    bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
    bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
    bool right = analogRead(lineSensorRight) > calibratedValue;
    bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
    bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
    bool left = analogRead(lineSensorLeft) > calibratedValue;
    bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
    bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;

    // Check if it stil sees all black
    if((right == 1 && innerRight == 1 && innerLeft == 1 && left == 1 && farLeft == 1 && outerLeft == 1) || (farRight == 1 && right == 1 && innerRight == 1 && innerLeft == 1 && left == 1 && farLeft == 1 && outerLeft == 1) || (outerRight == 1 && farRight == 1 && right == 1 && innerRight == 1 && innerLeft == 1 && left == 1 && farLeft == 1) || (farRight == 1 && right == 1 && innerRight == 1 && innerLeft == 1 && left == 1 && farLeft == 1)){
       drive(-170, -150);                                // Drive backwards to position cone
       delay(380);                                       // Delay
       drive(0,0);                                       // Stop
       openGripper();                                    // Open gripper
       delay(350);                                       // Delay
       drive(-170, -150);                                // Drive away
       delay(800);                                       // Delay
       endReached = true;                                // Reached the end
    }
  }
  // If detecting a path to the right
  else if (((outerRight == 1 && farLeft == 0) && (innerLeft == 1 || innerRight == 1)) || (outerRight == 1 && left == 1 && farLeft == 0 && outerLeft == 0) || (left == 1 && farRight == 1 && farLeft == 0) || (right == 1 && farRight == 1 && outerRight == 1 && farLeft == 0) || (innerRight == 1 && right == 1 && farRight == 1 && outerRight == 1 && farLeft == 0) || (left == 1 && innerLeft == 1 && outerRight == 1 && farLeft == 0) || (left == 1 && farRight == 1 && farLeft == 0) || (farRight == 1 && outerRight == 1 && farLeft == 0) || (outerRight == 1 && (innerRight == 1 || innerLeft == 1 || left == 1)  && farLeft == 0)){
    mySerial.println("Bocht naar rechts");
    moveStop();                                          // Stop
    isTurnRight = true;                                  // Turn right
  }
  // If line is centered
  else if (innerLeft == 1 && innerRight == 1){
    moveForward(200,190);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is slightly centered
  else if (innerLeft == 1 && innerRight == 0){
    moveForward(180,200);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is slightly centered
  else if (innerLeft == 0 && innerRight == 1){
    moveForward(200,165);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is slightly to the left
  else if (left == 1){
    moveForward(150,200);
    mySerial.println("Iets naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is slightly to the right
  else if (right == 1){
    moveForward(210,130);
    mySerial.println("Iets naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is to the left
  else if (farLeft == 1){
    moveForward(0,180);
    mySerial.println("Naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is to the right
  else if (farRight == 1){
    moveForward(180,0);
    mySerial.println("Naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is far to the right
  else if (outerRight == 1){
    moveForward(180,0);
    mySerial.println("Naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If line is far to the left
  else if (outerLeft == 1){
    moveForward(0,180);
    mySerial.println("Naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  // If the BattleBot lost the line
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
      rightLight();
      rotate(92);                                        // Turn right
      delay(160);
      bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
      bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
      bool right = analogRead(lineSensorRight) > calibratedValue;
      bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
      bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
      bool left = analogRead(lineSensorLeft) > calibratedValue;
      bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
      bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
      if(outerRight == 0 && farRight == 0 && right == 0 && innerRight == 0 && innerLeft == 0 && left == 0 && farRight == 0 && outerRight == 0){
        leftLight();
        rotate(-195);                                    // Turn far left
        delay(160);
        bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
        bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
        bool right = analogRead(lineSensorRight) > calibratedValue;
        bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
        bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
        bool left = analogRead(lineSensorLeft) > calibratedValue;
        bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
        bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
        if(outerRight == 0 && farRight == 0 && right == 0 && innerRight == 0 && innerLeft == 0 && left == 0 && farRight == 0 && outerRight == 0){
          leftLight();
          rotate(-92);                                   // Turn to the left to drive back
          delay(100);
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

void rotate(int rotation){
    int counter = 0;
    while(counter < (int)((135.0/360.0) * abs(rotation))){ 
                                                          // 135.0 / 360.0
        if(rotation > 0){
            drive(170, -170);
        }
        else{
            drive(-170, 170);
        }
        leftRotationState = digitalRead(leftRotationPin);
        rightRotationState = digitalRead(rightRotationPin);
        if (leftRotationState != leftRotationLastState || rightRotationState != rightRotationLastState){
            counter++;
        }
        leftRotationLastState = leftRotationState;
        rightRotationLastState = rightRotationState;
    }
    drive(0, 0);
}

void drive(int left, int right){
  if(moving == false && left < 150 && left > 0 && right < 150 && right > 0){
    analogWrite(rightMotorForward, 155);
    analogWrite(leftMotorForward, 155);
    delay(15);
    moving = true;
  }
  if(left < 0){
      analogWrite(leftMotorForward, 0);
      analogWrite(leftMotorBackward, abs(left));
  } 
  else{
      analogWrite(leftMotorForward, left);
      analogWrite(leftMotorBackward, 0);
  }
  if(right < 0){
      analogWrite(rightMotorForward, 0);
      analogWrite(rightMotorBackward, abs(right));
  } 
  else{
      analogWrite(rightMotorForward, right);
      analogWrite(rightMotorBackward, 0);
  }
  isDriving = abs(left) + abs(right) > 0;
  if(right == 0 && left == 0){
    moving = false;
  }
}

//===[Calibration]=========================================
void start(){
  if(shouldCalibrate)
  {
    int i;
    for (i = 0; i < calibrationTime; i++)
    {
      drive(200,200);
      delay(10);
      drive(99,75);
      qtr.calibrate();
      delay(30);
    }
    Serial.println("");
    Serial.println("Calibration complete");
    calibratedValue = getLineSensorSensitivity();
    Serial.println(calibratedValue);
  }
  if(outerLeft == 0 && farLeft == 0 && left == 0 && innerLeft == 0 && innerRight == 0 && right == 0 && farRight == 0 && outerRight == 0){     
                                                          // End of the black surface is reached
    delay(50);
    drive(0,0);
    closeGripper();
    delay(300);
    rotate(-100);
    drive(155,130);
    starting = false;
  }
}

//===[Gripper]=============================================
void openGripper(){
  analogWrite(gripperPin, 203);
}

void closeGripper(){
  analogWrite(gripperPin, 154);
}