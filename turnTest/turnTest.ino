#include <SoftwareSerial.h> 
  // Define the pins for the bluetooth module
SoftwareSerial mySerial(10, 8); // RX | TX 



//Pinnumbers
const int leftMotorPin1 = 3;
const int triggerPin = 4;
const int leftMotorPin2 = 5;
const int rightMotorPin2 = 6;
const int echoPin = 7;
const int rightMotorPin1 = 9;
const int rightRotationPin = 11;
const int leftRotationPin = 12;



//Variables
const int turn = 525;
const int around = 1025;
int left = 255;
int right = 255;
unsigned long duration;
unsigned int distance;
unsigned int valueRightSensor;
unsigned int valueLeftSensor;
int rotationState;
int rotationLastState;
int counter = 0;



//Setup
void setup() {
  Serial.begin(9600); 
  while (!Serial) {
    ;
  }
  mySerial.begin(9600);
  pinMode(triggerPin, OUTPUT);        //Ultra sonic distance sensor trigger
  pinMode(echoPin, INPUT);            //Ultra sonic distance sensor echo
  pinMode(rightMotorPin1, OUTPUT);    //Right motor
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);     //Left motor
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightRotationPin, INPUT);   //Right rotation sensor
  pinMode(leftRotationPin, INPUT);    //Left rotation sensor
}



//Loop
void loop() { 
  bluetooth();
  turnLeft();
  readRotation();
  if(counter = 6){
    brake();
  }
}



//Functions
void bluetooth(){
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

void detectWall() {                     //This function activates the ultra sonic distance sensor and it calculates the distance of the object 
  digitalWrite(triggerPin, LOW);        //in front of it in centimetres
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH); 
  distance = duration * 0.034/2;
}

void driveForward() {                   //This function activates both motors and will make the battlebot drive forward
   left = 200;
   right = 210;
   analogWrite(leftMotorPin2, left);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, right);
   digitalWrite(rightMotorPin1, LOW);   
}

void brake() {                          //This function deactivates both motors and will make the battlebot stop driving for a short time
   left = 0;
   right = 0;
   analogWrite(leftMotorPin2, left);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, right);
   digitalWrite(rightMotorPin1, LOW); 
   delay(500);
}

void turnRight() {                      //This function will make the battlebot make a 90 degree right turn
  left = 200;
  right = 210;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorPin1, right);
  delay(turn);
}

void turnLeft() {                       //This function will make the battlebot make a 90 degree left turn
  left = 200;
  right = 210;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  delay(turn);
}

void turnAround() {                     //This function will make the battlebot make a 180 degree turn
  left = 200;
  right = 210;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  delay(around);
}

void printRotationSensor() { 
  valueRightSensor = digitalRead(rightRotationPin);
  valueLeftSensor = digitalRead(leftRotationPin);
  Serial.print("R-");
  Serial.print(valueRightSensor);
  Serial.print("L-");
  Serial.println(valueLeftSensor);
}

void readRotation() {
    rotationState = digitalRead(leftRotationPin);
    if (rotationState != rotationLastState) {
        if (digitalRead(rightRotationPin) != rotationState) {
            counter--;
        } else {
            counter++;
        }
    }
    Serial.println(counter);
    rotationLastState = rotationState;
    mySerial.println(counter);
}
