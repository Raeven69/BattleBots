#include <Servo.h>
#include <SoftwareSerial.h>
#define bluetoothRX 2
#define bluetoothTX 3
SoftwareSerial mySerial(bluetoothRX, bluetoothTX);

const int leftMotorForward = 5;
const int leftMotorBackward = 10;
const int rightMotorForward = 9;
const int rightMotorBackward = 6;

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

  if(endReached == false && isTurnRight == false){
    followLine();
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
  else{
    moveStop();
  }

}

void turnAround(){
  moveForward(200,0);
  delay(850);
  moveStop();
  moveBackward(200,0);
  delay(920);
  moveStop();
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
  bool outerRight = analogRead(lineSensorOuterRight) > 950;
  bool farRight = analogRead(lineSensorFarRight) > 950;
  bool right = analogRead(lineSensorRight) > 950;
  bool innerRight = analogRead(lineSensorInnerRight) > 950;
  bool innerLeft = analogRead(lineSensorInnerLeft) > 950;
  bool left = analogRead(lineSensorLeft) > 950;
  bool farLeft = analogRead(lineSensorFarLeft) > 950;
  bool outerLeft = analogRead(lineSensorOuterLeft) > 950;
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

void followLine(){
  bool outerRight = analogRead(lineSensorOuterRight) > 950;
  bool farRight = analogRead(lineSensorFarRight) > 950;
  bool right = analogRead(lineSensorRight) > 950;
  bool innerRight = analogRead(lineSensorInnerRight) > 950;
  bool innerLeft = analogRead(lineSensorInnerLeft) > 950;
  bool left = analogRead(lineSensorLeft) > 950;
  bool farLeft = analogRead(lineSensorFarLeft) > 950;
  bool outerLeft = analogRead(lineSensorOuterLeft) > 950;

  if( mySerial.available() > 0){
    val = mySerial.read();
    mySerial.println(val);
    if( val == 'o'){
      endReached = true;
    }
  }

  if ((outerRight == 1 && (innerLeft == 1 || innerRight == 1)) || (outerRight == 1 && left == 1) || (left == 1 && farRight == 1) || (right == 1 && farRight == 1 && outerRight == 1) || (innerRight == 1 && right == 1 && farRight == 1 && outerRight == 1)) {
    mySerial.println("Bocht naar rechts");
    //turning = true;
    moveStop();
    isTurnRight = true;
  }
  if (innerLeft == 1 && innerRight == 1){
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
    moveForward(140,180);
    mySerial.println("Naar links");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (farRight == 1){
    moveForward(180,140);
    mySerial.println("Naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (outerRight == 1){
    moveForward(170,120);
    mySerial.println("Naar rechts");
    mySerial.println(String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
  }
  else if (outerLeft == 1){
    moveForward(120,170);
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
      moveBackward(177,177);
      delay(203);
      moveStop();
      delay(1000);
      bool outerRight = analogRead(lineSensorOuterRight) > 950;
      bool farRight = analogRead(lineSensorFarRight) > 950;
      bool right = analogRead(lineSensorRight) > 950;
      bool innerRight = analogRead(lineSensorInnerRight) > 950;
      bool innerLeft = analogRead(lineSensorInnerLeft) > 950;
      bool left = analogRead(lineSensorLeft) > 950;
      bool farLeft = analogRead(lineSensorFarLeft) > 950;
      bool outerLeft = analogRead(lineSensorOuterLeft) > 950;
      mySerial.println("LET OP: " + String(outerLeft) + " " + String(farLeft) + " " + String(left) + " " + String(innerLeft) + " " + String(innerRight) + " " + String(right) + " " + String(farRight) + " " + String(outerRight));
      if(outerLeft == 1 || farLeft == 1 || left == 1 || (outerLeft == 1 && farLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1 && innerLeft == 1) || (outerLeft == 1 && farLeft == 1 && left == 1)){
        mySerial.println("Ik zie wat links");
        //turnLeft();
      }
      else{
        turnAround();
        mySerial.println("Ik zie links niks");
      }
    }
  }
}
