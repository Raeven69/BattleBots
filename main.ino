#include <QTRSensors.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#define bluetoothRX 2
#define bluetoothTX 3
SoftwareSerial mySerial(bluetoothRX, bluetoothTX);

QTRSensors qtr;

const int leftMotorForward = 5; //5
const int leftMotorBackward = 6; //6
const int rightMotorForward = 9; //9
const int rightMotorBackward = 10; //10

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

int period = 1000;
unsigned long time_now = 0;

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
const int calibrationTime = 30; // in milliseconds * 20 (50 = 1 second)
const bool shouldCalibrate = true;

int getLineSensorSensitivity(int margin = 80)
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

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Open serial communication for the Bluetooth Serial Port
  mySerial.begin(9600);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);

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
  

  if(endReached == false && isTurnRight == false && isTurnLeft == false && rotating == false){
    followLine();
  }
  else if (rotating == true){
    rotate(180);
    rotating = false;
  }
  else if (isTurnRight == true && onceTurnRight == false){
    moveForward(170,0);
    delay(400);
    moveStop();
    onceTurnRight = true;
  }
  else if (isTurnRight == true && onceTurnRight == true){
    turnRight();
  }
  else if (isTurnLeft == true && onceTurnLeft == false){
    moveForward(0,170);
    delay(400);
    moveStop();
  }
  else if (isTurnLeft == true && onceTurnLeft == true){
    turnLeft();
  }
  else{
    moveStop();
  }

}

void turnAround(){
  moveBackward(0,200);
  delay(920);
  moveStop();
  rotating = false;
  isTurnLeft = true;
}

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
    moveForward(170,0);
  }
  if (innerLeft == 1 || (innerRight == 1 && innerLeft == 1)){
    moveStop();
    isTurnRight = false;
    onceTurnRight = false;
  }
}

void turnLeft(){
  if(innerRight == 0 || innerLeft == 0){
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
    moveForward(0,170);
  }
  if (innerRight == 1 || (innerRight == 1 && innerLeft == 1) || innerLeft == 1){
    moveStop();
    isTurnLeft = false;
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
  
  if (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1 && innerRight == 1 && right == 1 && farRight == 1){
    moveForward(110,190);
    mySerial.println("Naar links");
  }
  else if ((outerRight == 1 && (innerLeft == 1 || innerRight == 1)) || (outerRight == 1 && left == 1) || (left == 1 && farRight == 1) || (right == 1 && farRight == 1 && outerRight == 1) || (innerRight == 1 && right == 1 && farRight == 1 && outerRight == 1)) {
    mySerial.println("Bocht naar rechts");
    //turning = true;
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
    moveForward(120,190);
    mySerial.println("Iets naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (right == 1){
    moveForward(190,120);
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
      mySerial.println("In de achteruit");
      time_now = millis();
      while(millis() < time_now + period){
        bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
        bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
        bool right = analogRead(lineSensorRight) > calibratedValue;
        bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
        bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
        bool left = analogRead(lineSensorLeft) > calibratedValue;
        bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
        bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
        moveBackward(140,140);
        if(outerLeft == 1 || farLeft == 1 || left == 1 || (outerLeft == 1 && farLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1)){
          moveStop();
          mySerial.println("Ik zie wat links");
          isTurnLeft = true;
        }
      }
      moveStop();
      bool outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
      bool farRight = analogRead(lineSensorFarRight) > calibratedValue;
      bool right = analogRead(lineSensorRight) > calibratedValue;
      bool innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
      bool innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
      bool left = analogRead(lineSensorLeft) > calibratedValue;
      bool farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
      bool outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
      mySerial.println("LET OP: " + String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
      if(outerLeft == 1 || farLeft == 1 || (outerLeft == 1 && farLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1 && innerRight == 1 && right == 1 && farRight == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1 && innerRight == 1 && right == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1 && innerRight == 1)){
        mySerial.println("Ik zie wat links");
        mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
      }
      else{
        rotating = true;
        mySerial.println("Ik zie links niks");
      }
    }
  }
}

void rotate(int rotation) {
    int counter = 0;
    while (counter < (int)((140.0/360.0) * abs(rotation))) { // 140 was 135
        if (rotation > 0) {
            drive(150, -150);
        } else {
            drive(-150, 150);
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
