//Libraries
#include <SoftwareSerial.h>



//Pinnumbers
SoftwareSerial bluetooth(1, 0); // RX | TX  // Define the pins for the bluetooth module
const int grabberPin = 2;                    // Servo for grabbing the pion
const int servoPin = 9;                      // Servo for the ultra sonic distance sensor
const int leftMotorPin1 = 3;                 // Left motor backwards
const int triggerPin = 4;                    // Ultra sonic distance sensor trigger
const int leftMotorPin2 = 5;                 // Left motor forwards
const int rightMotorPin2 = 6;                // Right motor forwards
const int echoPin = 7;                       // Ultra sonic distance sensor echo
const int rightMotorPin1 = 11;               // Right motor backwards
const int rightRotationPin = 10;             // Right wheel rotation sensor
const int leftRotationPin = 12;              // Left wheel rotation sensor



//Variables
int counterRight = 0;                     // Counts the amount of pulses detected by the rotation sensor
int counterLeft = 0;
int rotationStateRight;                   // Saves the rotation state
int rotationStateLeft;
int rotationLastStateRight;               // Remembers the last rotation state
int rotationLastStateLeft;
int left = 255;                           // Speed of the left motor
int right = 255;                          // Speed of the right motor
unsigned long duration;                   // Time detected by the ultra sonic distance sensor
double distance;                          // Distance in centimetres
const double wheelCircumference = 20.41;  // Circumference of the wheel in centimetres
const double pulseDistance = 0.51;        // The amount of centimetres the battlebot will drive forward in 1 pulse
const int rotation = 40;                  // The amount of pulses for the wheel to reach one rotation
boolean drivingToWall = false;
boolean calibrating = false;
boolean turnedLeft = false;
boolean turnedRight = false;



//Setup
void setup() {
  servoFront();
  pinMode(servoPin, OUTPUT);
  pinMode(triggerPin, OUTPUT);    
  pinMode(echoPin, INPUT);           
  Serial.begin(9600);                     // Open serial communication                          
  bluetooth.begin(9600);                  // Open serial communication for the Bluetooth Serial Port
  pinMode(rightMotorPin1, OUTPUT);    
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);     
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightRotationPin, INPUT);   
  pinMode(leftRotationPin, INPUT);   
}



//Loop
void loop() {
  checkPerSquare();
}



//Functions
void detectWall() {                     // This function activates the ultra sonic distance sensor and it calculates the distance of the object 
  digitalWrite(triggerPin, LOW);        // in front of it in centimetres
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH); 
  distance = (duration * 0.034)/2;
  Serial.println(distance);
  delay(10);
}

void forwardOneSquare() {
  while(counterLeft < 50 && counterRight < 50) {
    readRotationLeft();
    readRotationRight();
    driveForward();
  }
}

void backwardOneSquare() {
  while(counterLeft < 50 && counterRight < 50) {
    readRotationLeft();
    readRotationRight();
    driveBackward();
  }
}

void calibrate() {
  
}

void calibrateLeft() {
  delay(500);
  while(counterLeft < 5) {
    readRotationLeft();
    backwardsLeft();
  }
  counterLeft = 0;
  brake();
  counterRight = 0;
  while(counterRight < 7) {
    readRotationRight();
    driveForward();
  }
  brake();
  counterRight = 0;
  while(counterRight < 4) {
    readRotationRight();
    backwardsRight();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  while(counterLeft < 4) {
    readRotationLeft();
    driveBackward();
  }
  counterLeft = 0;
}

void calibrateRight() {
  
}

void driveForward() {                   // This function activates both motors and will make the battlebot drive forward
   left = 252;
   right = 255;
   analogWrite(leftMotorPin2, left);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, right);
   digitalWrite(rightMotorPin1, LOW);   
}

void driveBackward() {                   // This function activates both motors and will make the battlebot drive backward
   left = 252;
   right = 255;
   analogWrite(leftMotorPin1, left);
   digitalWrite(leftMotorPin2, LOW);
   analogWrite(rightMotorPin1, right);
   digitalWrite(rightMotorPin2, LOW);   
}

void brake() {                          // This function deactivates both motors and will make the battlebot stop driving for a short time
   left = 0;
   right = 0;
   analogWrite(leftMotorPin2, LOW);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, LOW);
   digitalWrite(rightMotorPin1, LOW);
   delay(500); 
}

void stop() {                          // This function deactivates both motors and will make the battlebot stop driving
   left = 0;
   right = 0;
   analogWrite(leftMotorPin2, left);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, right);
   digitalWrite(rightMotorPin1, LOW); 
}

void turnRight() {                      // This function will make the battlebot make a 90 degree right turn
  delay(500);
  while(counterRight < 15) {   
    readRotationRight();
    backwardsRight();
  }
  counterRight = 0;
  brake();
  while(counterRight < 11) {
    readRotationRight();
    driveForward();
  }
  counterRight = 0;
  brake();
  while(counterLeft < 15) {
    readRotationLeft();
    forwardsLeft();
  }
  counterLeft = 0;
  counterRight = 0;
  brake();
  while(counterLeft < 16) {
    readRotationLeft();
    driveBackward();
  }
  counterLeft = 0;
}

void turnLeft() {                       // This function will make the battlebot make a 90 degree left turn
  delay(500);
  while(counterLeft < 15) {
    readRotationLeft();
    backwardsLeft();
  }
  brake();
  counterRight = 0;
  while(counterRight < 11) {
    readRotationRight();
    driveForward();
  }
  brake();
  counterRight = 0;
  while(counterRight < 15) {
    readRotationRight();
    forwardsRight();
  }
  counterLeft = 0;
  counterRight = 0;
  brake();
  while(counterLeft < 16) {
    readRotationLeft();
    driveBackward();
  }
  counterLeft = 0;
}

void backwardsLeft() {                 // This function will make the left wheel turn backward
  left = 255;
  right = 255;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
}

void forwardsLeft() {                   // This function will make the left wheel turn forward
  left = 255;
  right = 255;
  analogWrite(leftMotorPin2, right);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
}

void backwardsRight() {                 // This function will make the right wheel turn backward
  left = 255;
  right = 255;
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin1, right);
  digitalWrite(rightMotorPin2, LOW);
}

void forwardsRight() {                  // This function will make the right wheel turn forward
  left = 255;
  right = 255;
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
}

void turnAround() {                     // This function will make the battlebot make a 180 degree turn
  left = 255;
  right = 255;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  delay(1025);
}

void readRotationRight() {              // This function will read the pulses of the right rotation sensor
    rotationStateRight = digitalRead(rightRotationPin);
    if(rotationStateRight != rotationLastStateRight) {
        counterRight++;   
    }
    rotationLastStateRight = rotationStateRight;
}

void readRotationLeft() {               // This function will read the pulses of the left rotation sensor
    rotationStateLeft = digitalRead(leftRotationPin);
    if(rotationStateLeft != rotationLastStateLeft) {
        counterLeft++;   
    }
    rotationLastStateLeft = rotationStateLeft;
}

void servoLeft() {                   // This function will make the servo turn to the left 
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(2600); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 650 - 0deg; 1650 - 90deg; 2600 - 180deg
}

void servoFront() {                  // This function will make the servo turn to the front
  // A pulse each 20ms  
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(1650); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 650 - 0deg; 1650 - 90deg; 2600 - 180deg
}

void servoRight() {                   // This function will make the servo turn to the right
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(650); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 650 - 0deg; 1650 - 90deg; 2600 - 180deg
}

void checkForPath() {
  detectWall();
  if(distance > 0 && distance < 8 && drivingToWall == false) {// Check for a wall and check if the function is already activated
    counterRight = 0;                                         // Reset the right pulse count
    counterLeft = 0;                                          // Reset the left pulse count
    drivingToWall = true;                                     // Let the arduino know its driving to a wall
    brake();
    servoLeft();                                              // Check for path on the left side
    delay(800);
    detectWall();
    if(distance > 0 && distance < 20) {                       // If there is no path, check for a path on the right side
      servoRight();
      delay(800);
      detectWall();
      if(distance > 0 && distance < 20) {                     // If there is no path, drive backwards the same distance, and make the same last made turn to position the bot
        servoFront();
        turnAround();
        brake();
        drivingToWall = false;
      }
      else {                                                  // If there is a path, make a 90 degree right turn
        servoFront();
        turnRight();
        brake();
        drivingToWall = false;
      }
    }
    else {                                                    // If there is a path, make a 90 degree left turn
      servoFront();
      turnLeft();
      brake();
      drivingToWall = false;
    }
  }
  else {
    driveForward();                                           // If there is no wall, drive forward
    counterRight = 0;
    counterLeft = 0;
  }
}

void checkPerSquare() {
  if(calibrating == false) {
    counterLeft = 0;
    counterRight = 0;
    calibrating = true;
    brake();
    servoLeft();
    delay(500);
    detectWall();   
    if(distance < 16) {
      brake();
      servoRight();
      delay(500);
      detectWall();
      if(distance < 16) {
        brake();
        servoFront();
        delay(500);
        detectWall();
        if(distance < 8) {
          brake();
          backwardOneSquare();
          brake();
          counterLeft = 0;
          counterRight = 0;
          turnRight();
          brake();
          forwardOneSquare();
          calibrating = false;
        }
        else {
          forwardOneSquare();
          calibrating = false;
        }
      }
      else {
        servoFront();
        delay(500);
        turnRight();
        turnedRight = true;
        brake();
        forwardOneSquare();
        calibrating = false;
      }
    }
    else {
      servoFront();
      delay(500);
      turnLeft();
      turnedLeft = true;
      brake();
      forwardOneSquare();
      calibrating = false;
    }
  }
  else {
    calibrating = false;
  }
  //check if the bot is in the middle of the path
  //check left, if between 4 and 25 cm, calibrate to the left
  //check right, if between 4 and 25 cm, calibrate to the right
  //check for a path on the left, if path then turn left, if no path
  //check for a path on the right, if path then turn right, if no path, go forward one square
  //if no path in the front, turn around and remember it is a dead end
}
