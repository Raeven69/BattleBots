//Libraries
#include <SoftwareSerial.h>
#include <Servo.h>



//Pinnumbers
SoftwareSerial bluetooth(13, 8); // RX | TX  // Define the pins for the bluetooth module
Servo grabber;                               // Define the name of the servo
const int grabberPin = 9;                    // Servo 
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
bool checkingSides = false;
const double wheelCircumference = 20.41;  // Circumference of the wheel in centimetres
const double pulseDistance = 0.51;        // The amount of centimetres the battlebot will drive forward in 1 pulse
const int rotation = 40;                  // The amount of pulses for the wheel to reach one rotation
double distanceFromWall;                  // The amount of distance until the battlebot hits a wall
int pulseCount;                           // The amount of pulses the wheels need to rotate to reach a certain part of the maze
boolean drivingToWall = false;



//Setup
void setup() {
  grabberFront();
  pinMode(triggerPin, OUTPUT);    
  pinMode(echoPin, INPUT);
  grabber.attach(grabberPin);            
  Serial.begin(9600);                  // Open serial communications and wait for port to open:                          
  while (!Serial) {
    ;
  }
  bluetooth.begin(9600);               // Open serial communication for the Bluetooth Serial Port
  pinMode(rightMotorPin1, OUTPUT);    
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);     
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightRotationPin, INPUT);   
  pinMode(leftRotationPin, INPUT);   
}



//Loop
void loop() {
  driveForward();
}



//Functions
void enableBluetooth() {                // This function enables bluetooth and will send all information from the bluetooth device to the serial monitor
  if (bluetooth.available()) {          // If any data is available at the Bluetooth Serial Port
    Serial.write(bluetooth.read());     // Write this data to the Serial Monitor
  }
  if (Serial.available()) {             // If any data is sent via the Serial Monitor
    bluetooth.write(Serial.read());     // Send this data via the Bluetooth Serial Port
  }
}

void detectWall() {                     // This function activates the ultra sonic distance sensor and it calculates the distance of the object 
  digitalWrite(triggerPin, LOW);        // in front of it in centimetres
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH); 
  distance = duration * 0.034/2;
}

void driveForward() {                   // This function activates both motors and will make the battlebot drive forward
   left = 200;
   right = 210;
   analogWrite(leftMotorPin2, left);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, right);
   digitalWrite(rightMotorPin1, LOW);   
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
    left = 200;
    right = 200;
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
    analogWrite(rightMotorPin1, right);
  }while(counterLeft < 15) {
    readRotationLeft();
    left = 200;
    right = 200;
    digitalWrite(leftMotorPin1, LOW);
    analogWrite(leftMotorPin2, left);
    digitalWrite(rightMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
  }
}

void turnLeft() {                       // This function will make the battlebot make a 90 degree left turn
  delay(500);
  while(counterLeft < 15) {
    readRotationLeft();
    left = 200;
    right = 200;
    digitalWrite(leftMotorPin2, LOW);
    analogWrite(leftMotorPin1, left);
    digitalWrite(rightMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
  }
  while(counterRight < 15) {
    readRotationRight();
    left = 200;
    right = 200;
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(rightMotorPin1, LOW);
    analogWrite(rightMotorPin2, right);
  }
}

void turnAround() {                     // This function will make the battlebot make a 180 degree turn
  left = 200;
  right = 200;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  delay(1025);
}

void readRotationRight() {
    rotationStateRight = digitalRead(rightRotationPin);
    if(rotationStateRight != rotationLastStateRight) {
        counterRight++;   
    }
    Serial.print("pulsecount right: ");
    Serial.println(counterRight);
    rotationLastStateRight = rotationStateRight;
}

void readRotationLeft() {
    rotationStateLeft = digitalRead(leftRotationPin);
    if(rotationStateLeft != rotationLastStateLeft) {
        counterLeft++;   
    }
    Serial.print("pulsecount left: ");
    Serial.println(counterLeft);
    rotationLastStateLeft = rotationStateLeft;
}

void grabberLeft() {
  grabber.write(180);
  delay(600);
}

void grabberFront() {
  grabber.write(110);
}

void grabberRight() {
  grabber.write(20);
  delay(600);
}

void checkForPath() {
   if(distance > 0 && distance < 18 && checkingSides == false) {
    checkingSides = true;
    Serial.println("Wall detected");
    brake();
    grabberLeft();
    detectWall();
    if(distance < 18) {
      Serial.println("Left wall detected");
      grabberRight();
      detectWall();
      if(distance < 18) {
        Serial.println("Right wall detected");
        Serial.println("No path found, turning around");
        grabberFront();
        turnAround();
        brake();
        checkingSides = false;
      }
      else {
        Serial.println("Path found on the right side, turning right");
        grabberFront();
        turnRight();
        brake();
        checkingSides = false;
      }
    }
    else {
      Serial.println("Path found on the left side, turning left");
      grabberFront();
      turnLeft();
      brake();
      counterRight = 0;
      checkingSides = false;
    }
  }
  else {
    driveForward();
    grabberFront();
  }
}

void solveMaze() {
  detectWall();
  if(distance < 21 && drivingToWall == false) {
    counterRight = 0;
    counterLeft = 0;
    drivingToWall = true;
    brake();
    distanceFromWall = distance;
    pulseCount = (distanceFromWall / pulseDistance - 20);     // Calculate how many pulses until hit wall
    Serial.print("Pulses to drive is: ");
    Serial.println(pulseCount);
    while(counterRight <= pulseCount) {                       // Drive the amount of pulses
      driveForward();
      readRotationRight();
    }
    counterRight = 0;
    brake();
    grabberLeft();                                            // Check for path on the left side
    delay(800);
    detectWall();
    if(distance < 15) {                                       // If there is no path, check for a path on the right side
      grabberRight();
      delay(800);
      detectWall();
      if(distance < 15) {                                     // If there is no path, drive backwards the same distance, and make the same last made turn to position the bot
        grabberFront();
        turnAround();
        brake();
        drivingToWall = false;
      }
      else {                                                  // If there is a path, make a 90 degree right turn
        grabberFront();
        turnRight();
        brake();
        drivingToWall = false;
      }
    }
    else {                                                    // If there is a path, make a 90 degree left turn
      grabberFront();
      turnLeft();
      brake();
      drivingToWall = false;
    }
  
    
    //check the right side for a path and make a perfect 90 degree right turn.
  }
  else {
    driveForward();
    counterRight = 0;
    counterLeft = 0;
  }
}
