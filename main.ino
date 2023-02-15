#include <Servo.h>
#include <SoftwareSerial.h>
#define bluetoothRX 2
#define bluetoothTX 3
SoftwareSerial mySerial(bluetoothRX, bluetoothTX);

const int leftMotorForward = 5;
const int leftMotorBackward = 10;
const int rightMotorForward = 9;
const int rightMotorBackward = 6;

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

int period = 600;
unsigned long time_now = 0;

int leftRotationState;
int lastLeftRotationState;
int rightRotationState;
int lastRightRotationState;
int leftRotationCounter;
int rightRotationCounter;

bool leftRotating = false;
bool rotating = false;

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

}


void loop() {
  // put your main code here, to run repeatedly:
  

  if(endReached == false && isTurnRight == false && isTurnLeft == false && rotating == false){
    followLine();
  }
  else if (rotating == true){
    turnAround();
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
    onceTurnLeft = true;
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
  bool outerRight = analogRead(lineSensorOuterRight) > 850;
  bool farRight = analogRead(lineSensorFarRight) > 850;
  bool right = analogRead(lineSensorRight) > 850;
  bool innerRight = analogRead(lineSensorInnerRight) > 850;
  bool innerLeft = analogRead(lineSensorInnerLeft) > 850;
  bool left = analogRead(lineSensorLeft) > 850;
  bool farLeft = analogRead(lineSensorFarLeft) > 850;
  bool outerLeft = analogRead(lineSensorOuterLeft) > 850;
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
  bool outerRight = analogRead(lineSensorOuterRight) > 850;
  bool farRight = analogRead(lineSensorFarRight) > 850;
  bool right = analogRead(lineSensorRight) > 850;
  bool innerRight = analogRead(lineSensorInnerRight) > 850;
  bool innerLeft = analogRead(lineSensorInnerLeft) > 850;
  bool left = analogRead(lineSensorLeft) > 850;
  bool farLeft = analogRead(lineSensorFarLeft) > 850;
  bool outerLeft = analogRead(lineSensorOuterLeft) > 850;
  if(innerRight == 0 || innerLeft == 0){
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
    moveForward(0,170);
  }
  if (innerRight == 1 || (innerRight == 1 && innerLeft == 1)){
    moveStop();
    isTurnLeft = false;
    onceTurnLeft = false;
  }
}

void followLine(){
  bool outerRight = analogRead(lineSensorOuterRight) > 750;
  bool farRight = analogRead(lineSensorFarRight) > 750;
  bool right = analogRead(lineSensorRight) > 750;
  bool innerRight = analogRead(lineSensorInnerRight) > 750;
  bool innerLeft = analogRead(lineSensorInnerLeft) > 750;
  bool left = analogRead(lineSensorLeft) > 750;
  bool farLeft = analogRead(lineSensorFarLeft) > 750;
  bool outerLeft = analogRead(lineSensorOuterLeft) > 750;

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
    moveForward(200,200);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (innerLeft == 0 && innerRight == 1){
    moveForward(200,200);
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (left == 1){
    moveForward(160,190);
    mySerial.println("Iets naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (right == 1){
    moveForward(190,160);
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
        bool outerRight = analogRead(lineSensorOuterRight) > 850;
        bool farRight = analogRead(lineSensorFarRight) > 850;
        bool right = analogRead(lineSensorRight) > 850;
        bool innerRight = analogRead(lineSensorInnerRight) > 850;
        bool innerLeft = analogRead(lineSensorInnerLeft) > 850;
        bool left = analogRead(lineSensorLeft) > 850;
        bool farLeft = analogRead(lineSensorFarLeft) > 850;
        bool outerLeft = analogRead(lineSensorOuterLeft) > 850;
        moveBackward(140,140);
        if(outerLeft == 1 || farLeft == 1 || left == 1 || (outerLeft == 1 && farLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1)){
          moveStop();
          mySerial.println("Ik zie wat links");
          isTurnLeft = true;
        }
      }
      moveStop();
      bool outerRight = analogRead(lineSensorOuterRight) > 850;
      bool farRight = analogRead(lineSensorFarRight) > 850;
      bool right = analogRead(lineSensorRight) > 850;
      bool innerRight = analogRead(lineSensorInnerRight) > 850;
      bool innerLeft = analogRead(lineSensorInnerLeft) > 850;
      bool left = analogRead(lineSensorLeft) > 850;
      bool farLeft = analogRead(lineSensorFarLeft) > 850;
      bool outerLeft = analogRead(lineSensorOuterLeft) > 850;
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
