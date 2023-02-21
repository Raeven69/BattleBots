#include <SoftwareSerial.h> 
  // Define the pins for the bluetooth module
SoftwareSerial mySerial(13, 8); // RX | TX 



//Pinnumbers
const int leftMotorPin1 = 3;
const int triggerPin = 4;
const int leftMotorPin2 = 5;
const int rightMotorPin2 = 6;
const int echoPin = 7;
const int rightMotorPin1 = 11;
const int rightRotationPin = 10;
const int leftRotationPin = 12;



//Variables
int left = 255;
int right = 255;
unsigned long duration;
unsigned int distance;
unsigned int valueRightSensor;
unsigned int valueLeftSensor;
int rotationState;
int rotationLastState;
int counter = 0;
unsigned int turnDuration;
unsigned int turnAroundDuration;


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
  brake();
  readRotation();
  while(counter < 17) {
    readRotation();
    turnLeft();
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
  duration = pulseIn(rightRotationPin, HIGH);
  turnDuration = duration * 0.0065;
  delay(turnDuration);
}

void turnLeft() {                       //This function will make the battlebot make a 90 degree left turn
  left = 200;
  right = 210;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  
}

void turnAround() {                     //This function will make the battlebot make a 180 degree turn
  left = 200;
  right = 210;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  duration = pulseIn(leftRotationPin, HIGH);
  turnAroundDuration = duration * 0.0145;
  delay(turnAroundDuration);
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
    rotationState = digitalRead(rightRotationPin);
    if (rotationState != rotationLastState) {
        counter++;   
    }
    Serial.println(counter);
    rotationLastState = rotationState;
    mySerial.println(counter);
}
