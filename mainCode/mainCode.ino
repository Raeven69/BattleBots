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
boolean checking = false;
boolean turnedLeft = false;
boolean turnedRight = false;
boolean turnedAround = false;
boolean started = false;




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
  if(started == false) {
    startPosition();
  }
  hugRightWall();
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

void forwardOneSquare() {               // This function makes the battlebot drive on square forward in the maze
  while(counterLeft < 48) {
    readRotationLeft();
    driveForward();
  }
}

void startPosition() {                  // This function makes the battlebot drive drive to the right position in the square
  while(counterLeft < 20) {
    readRotationLeft();
    driveForward();
    started = true;
  }
}

void backwardOneSquare() {              // This function makes the battlebot drive on square backward in the maze
  while(counterLeft < 50 && counterRight < 50) {
    readRotationLeft();
    readRotationRight();
    driveBackward();
  }
}

void calibrate() {                            // Check if the bot is in the middle of the path
  servoLeft();                                // Position the servo to the left side
  delay(500);                                 
  detectWall();                               // Activate the ultra sonic distance sensor
  if(distance > 12 && distance < 25) {        // Check left, if between 12 and 25cm, calibrate to the left
    calibrateLeft();                        
    servoFront();
  }
  delay(500);
  servoRight();
  delay(500);
  detectWall();
  if(distance > 12 && distance < 25) {         // Check right, if between 12 and 25 cm, calibrate to the right
    calibrateRight();
    servoFront();
  }
  servoFront();
}

void calibrateLeft() {                        // This function makes the battle bot calibrate to the left
  delay(500);
  while(counterLeft < 5) {
    readRotationLeft();
    backwardsLeft();
  }
  counterLeft = 0;
  counterRight = 0;
  brake();
  counterRight = 0;
  while(counterRight < 7) {
    readRotationRight();
    driveForward();
  }
  brake();
  counterRight = 0;
  counterLeft = 0;
  while(counterRight < 4) {
    readRotationRight();
    backwardsRight();
  }
  brake();
  counterRight = 0;
  counterLeft = 0;
  while(counterLeft < 4) {
    readRotationLeft();
    driveBackward();
  }
  counterLeft = 0;
  counterRight = 0;
}

void calibrateRight() {                           // This function makes the battlebot calibrate to the right
  delay(500);
  while(counterRight < 5) {
    readRotationRight();
    backwardsRight();
  }
  counterLeft = 0;
  counterRight = 0;
  brake();
  counterRight = 0;
  while(counterRight < 7) {
    readRotationRight();
    driveForward();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  while(counterLeft < 4) {
    readRotationLeft();
    backwardsLeft();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  while(counterLeft < 4) {
    readRotationLeft();
    driveBackward();
  }
  counterLeft = 0;
  counterRight = 0;
}

void driveForward() {                   // This function activates both motors and will make the battlebot drive forward
  left = 255;
  right = 255;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);   
}

void driveBackward() {                   // This function activates both motors and will make the battlebot drive backward
  left = 255;
  right = 253;
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
  delay(200); 
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
  while(counterRight < 17) {
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
  while(counterLeft < 25) {
    readRotationLeft();
    driveForward();
  }
  counterLeft = 0;
}

void turnLeft() {                       // This function will make the battlebot make a 90 degree left turn
  delay(500);
  while(counterLeft < 16) {
    readRotationLeft();
    backwardsLeft();
  }
  brake();
  counterRight = 0;
  while(counterRight < 17) {
    readRotationRight();
    driveForward();
  }
  brake();
  counterRight = 0;
  while(counterRight < 16) {
    readRotationRight();
    forwardsRight();
  }
  counterLeft = 0;
  counterRight = 0;
  brake();
  while(counterLeft < 25) {
    readRotationLeft();
    driveForward();
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

void turnAround() {                 // This function will make the battlebot make a 180 degree turn to the left
  delay(500);
  while(counterLeft < 32) {
    readRotationLeft();
    backwardsLeft();
  }
  brake();
  while(counterRight < 32) {
    readRotationRight();
    forwardsRight();
  }
  brake();
  counterRight = 0;
  counterLeft = 0;
  while(counterRight < 13) {
    readRotationRight();
    driveForward();
  }
  counterRight = 0;
  counterLeft = 0;
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
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

void servoFront() {                  // This function will make the servo turn to the front
  // A pulse each 20ms  
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(1650); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

void servoRight() {                   // This function will make the servo turn to the right
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(600); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

void checkForPathLeft() {
  if(checking == false) {                                     // Check if the function is already running
    //brake();
    //calibrate();                                              // If necessary, calibrate
    //brake();
    counterLeft = 0;                                          // Reset the left pulse count
    counterRight = 0;                                         // Reset the right pulse count
    checking = true;                                          
    brake();    
    servoLeft();                                              // Position the servo to look to the left
    delay(500);
    detectWall();                                             // Activates the ultra sonic distance sensor and checks if there is a wall
    if(distance < 20) {                                       // If a wall is detected on the left side, position the servo to look to the right
      brake();
      servoRight();
      delay(500);
      detectWall();
      if(distance < 20) {                                     // If a wall is detected on the right side, position the servo to the front
        brake();
        servoFront();
        delay(500);
        detectWall(); 
        if(distance < 15) {                                   // If a wall is detected on the front side, the battlebot will turn around
          brake();
          counterLeft = 0;
          counterRight = 0;
          turnAround();
          turnedAround = true;
          turnedLeft = false;
          turnedRight = false;
          brake();
          checking = false;
        }
        else {                                                // If no path is detected on the sides, drive one square forward
          forwardOneSquare();
          checking = false;
        }
      }
      else {                                                 // If a path is detected on the right side, turn to the right
        servoFront();
        delay(500);
        turnRight();
        turnedRight = true;
        turnedLeft = false;
        turnedAround = false;
        brake();
        checking = false;
      }
    }
    else {                                                  // If a path is detected on the left side, turn to the left
      servoFront();
      delay(500);
      turnLeft();
      turnedLeft = true;
      turnedRight = false;
      turnedAround = false;
      brake();
      checking = false;
    }
  }
  else {
    checking = false;
  }
}

void hugRightWall() {
  if(checking == false) {                                     // Check if the function is already running
    counterLeft = 0;                                          // Reset the left pulse count
    counterRight = 0;                                         // Reset the right pulse count
    checking = true;                                          
    brake();    
    servoRight();                                              // Position the servo to look to the left
    delay(300);
    detectWall();                                             // Activates the ultra sonic distance sensor and checks if there is a wall
    if(distance < 20) {                                       // If a wall is detected on the left side, position the servo to look to the right
      brake();
      servoFront();
      delay(300);
      detectWall();
      if(distance < 15) {                                     // If a wall is detected on the right side, position the servo to the front
        brake();
        servoLeft();
        delay(300);
        detectWall(); 
        if(distance < 20) {                                   // If a wall is detected on the front side, the battlebot will turn around
          brake();
          servoFront();
          counterLeft = 0;
          counterRight = 0;
          turnAround();
          brake();
          checking = false;
        }
        else {                                                // If no path is detected on the sides, drive one square forward
          servoFront();
          turnLeft();
          checking = false;
        }
      }
      else {                                                 // If a path is detected on the right side, turn to the right
        servoFront();
        delay(300);
        forwardOneSquare();
        brake();
        checking = false;
      }
    }
    else {                                                  // If a path is detected on the left side, turn to the left
      servoFront();
      delay(300);
      turnRight();
      brake();
      checking = false;
    }
  }
  else {
    checking = false;
  }
}

void checkForPathFront() {
  if(checking == false) {                                     // Check if the function is already running
    //brake();
    //calibrate();                                            // If necessary, calibrate
    //brake();
    counterLeft = 0;                                          // Reset the left pulse count
    counterRight = 0;                                         // Reset the right pulse count
    checking = true;                                          
    brake();    
    servoFront();                                             // Position the servo to look to the left
    delay(500);
    detectWall();                                             // Activates the ultra sonic distance sensor and checks if there is a wall
    if(distance < 20) {                                       // If a wall is detected on the left side, position the servo to look to the right
      brake();
      servoLeft();
      delay(500);
      detectWall();
      if(distance < 20) {                                     // If a wall is detected on the right side, position the servo to the front
        brake();
        servoRight();
        delay(500);
        detectWall(); 
        if(distance < 15) {                                   // If a wall is detected on the front side, the battlebot will turn around
          brake();
          counterLeft = 0;
          counterRight = 0;
          turnAround();
          turnedAround = true;
          turnedLeft = false;
          turnedRight = false;
          brake();
          checking = false;
        }
        else {                                                // If no path is detected on the sides, drive one square forward
          servoFront();
          delay(500);
          turnRight();
          turnedRight = true;
          turnedLeft = false;
          turnedRight = false;
          brake();
          checking = false;
        }
      }
      else {                                                 // If a path is detected on the right side, turn to the right
        servoFront();
        delay(500);
        turnLeft();
        turnedLeft = true;
        turnedRight = false;
        turnedAround = false;
        brake();
        checking = false;
      }
    }
    else {                                                  // If a path is detected on the left side, turn to the left
      servoFront();
      delay(500);
      brake();
      forwardOneSquare();
      turnedAround = false;
      checking = false;
    }
  }
  else {
    checking = false;
  }
}
