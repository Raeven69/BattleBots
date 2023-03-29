/************************************
***            INF1A              ***
***            29-3-2023          ***
***            Versie 0.1         ***
***            Jannes             ***
************************************/


//Libraries
#include <QTRSensors.h>
#include <Adafruit_NeoPixel.h>               // Library used to control the NeoPixels

QTRSensors qtr; 


//Pinnumbers
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
const int ledPin = 13;                       // Neopixels
const int lineSensorOuterRight = A0;         // Line sensors
const int lineSensorFarRight = A1;
const int lineSensorRight = A2;
const int lineSensorInnerRight = A3;
const int lineSensorInnerLeft = A4;
const int lineSensorLeft = A5;
const int lineSensorFarLeft = A6;
const int lineSensorOuterLeft = A7;
const int ledCount = 4;                      // Amount of neopixels

Adafruit_NeoPixel strip(ledCount, ledPin, NEO_GRB + NEO_KHZ800);



//Variables

// Grabber variables //
bool grabberIsClosed = false;

// Rotation sensor variables //
int counterRight = 0;                     // Counts the amount of pulses detected by the rotation sensor
int counterLeft = 0;
int rotationStateRight;                   // Saves the rotation state
int rotationStateLeft;
int rotationLastStateRight;               // Remembers the last rotation state
int rotationLastStateLeft;

// Wheel speed variables //
int left = 255;                           // Speed of the left motor
int right = 255;                          // Speed of the right motor

// NeoPixel variables
unsigned long timeNe = 0;                 // Start value for millis Neopixel

// Distance sensor variables
unsigned long duration;                   // Time detected by the ultra sonic distance sensor
double distance;                          // Distance in centimetres

// Line sensor variables //
uint16_t sensorValues[8];                 // An array to store the current values of the sensors in
bool isOnLine;                            // Variable to keep track of whether the robot is on the line or not
uint16_t position;                        // Variable to store the position of the line relative to the sensors
bool wasOnLines = false;                  // Calibration line counting variables
long lastAllLines = 0;                    // Variable for keeping the last time the robot was on a black line
boolean endReached = false;
boolean isFinished = false;
boolean wasAllOnLine = false;
long lastAllOnLine = 0;

// Other variables //
boolean checking = false;                 // Variable to check if the robot is checking for a path               
const int safeTurnDistance = 25;
const int servoDelay = 300;



//Setup
void setup() {
  // Set the servo for the ultra sonic distance sensor to the front
  servoFront();
 
  // Open serial communication
  Serial.begin(9600);
  
  // Define inputs and outputs                     
  pinMode(servoPin, OUTPUT);
  pinMode(triggerPin, OUTPUT);    
  pinMode(echoPin, INPUT);                                     
  pinMode(rightMotorPin1, OUTPUT);    
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);     
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightRotationPin, INPUT);   
  pinMode(leftRotationPin, INPUT);   
  pinMode(lineSensorOuterRight, INPUT);
  pinMode(lineSensorFarRight, INPUT);
  pinMode(lineSensorRight, INPUT);
  pinMode(lineSensorInnerRight, INPUT);
  pinMode(lineSensorInnerLeft, INPUT);
  pinMode(lineSensorLeft, INPUT);
  pinMode(lineSensorFarLeft, INPUT);
  pinMode(lineSensorOuterLeft, INPUT);

  // Define the NeoPixels
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    strip.begin();                        // Initialize NeoPixel strip
    strip.show();                         // Turn OFF all pixels
    strip.setBrightness(100);             // Brightness (0-255)

  pinMode(ledPin, OUTPUT);                // Set ledPin to OUTPUT
  
  // Start calibration
  grabberOpen();
  calibrateSensor(); 
}



//Loop
void loop() {
    if(!isFinished) {
      hugRightWall();
    }
    else {
      positionPawn();
    }
}




// Functions


// NeoPixel functions //

// Light when driving forwards
void forwardLight(){
  strip.clear();
  strip.setPixelColor(2, strip.Color(255, 255, 255));
  strip.setPixelColor(3, strip.Color(255, 255, 255));  
  strip.setPixelColor(0, strip.Color(255, 255, 255)); 
  strip.setPixelColor(1, strip.Color(255, 255, 255)); 
  strip.show();
}

// Light when driving backwards
void backwardLight(){
   strip.clear();
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0)); 
  strip.setPixelColor(2, strip.Color(0, 255, 0));
  strip.setPixelColor(3, strip.Color(0, 255, 0));   
  strip.show();
}

// Light when turning left
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

// Light when turning right
void rightLight(){
  strip.clear();
  if(millis() >= timeNe + 150){
    timeNe = millis() + 150;
    
    strip.setPixelColor(1, strip.Color(255, 255, 50));
    strip.setPixelColor(2, strip.Color(255, 255, 50));
    }
  strip.show();
}

// Light when calibrating
void calibrateLight(){                     
  strip.clear();
  if(millis() >= timeNe + 150){
    timeNe = millis() + 150;
    
    strip.setPixelColor(1, strip.Color(255, 255, 50));
    strip.setPixelColor(2, strip.Color(255, 255, 50));
    strip.setPixelColor(3, strip.Color(255, 255, 50));
    strip.setPixelColor(0, strip.Color(255, 255, 50));
    }
  strip.show();
}


// Ultra sonic distance sensor functions //

// This function activates the ultra sonic distance sensor and it calculates the distance of the object in front of it in centimetres
void detectWall() {                     
  digitalWrite(triggerPin, LOW);        
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


// Driving functions //

// This function makes the battlebot drive one square forward in the maze
void forwardOneSquare() {               
  while(counterLeft < 1) {
    readRotationLeft();
    calibrateDrive();
  }
  counterLeft = 0;
  while(counterLeft < 52) {
    readRotationLeft();
    driveForward();
  }
}

// This function makes the battlebot drive half a square forward in the maze
void forwardHalfSquare() {               
  while(counterLeft < 1) {
    readRotationLeft();
    calibrateDrive();
  }
  counterLeft = 0;
  while(counterLeft < 29) {
    readRotationLeft();
    driveStraightForward();
  }
}

// This function makes the battlebot drive inside off the maze after calibrating
void forwardInMaze() {               
  while(counterLeft < 1) {
    readRotationLeft();
    calibrateDrive();
  }
  counterLeft = 0;
  while(counterLeft < 59) {
    readRotationLeft();
    driveStraightForward();
  }
}

// This function activates both motors and will make the battlebot drive forward
void driveForward() {                   
  forwardLight();
  left = 240;
  right = 233;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);  
}

// This function activates both motors and will make the battlebot drive forward while driving for a short amount of time
void driveStraightForward() {                   
  forwardLight();
  left = 239;
  right = 232;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);  
}

// This function deactivates both motors and will make the battlebot stop driving for a short time
void brake() {                          
  backwardLight();
  left = 0;
  right = 0;
  analogWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
  delay(250); 
}

// This function deactivates both motors and will make the battlebot stop driving
void stopMoving() {                          
  backwardLight();
  left = 0;
  right = 0;
  analogWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW); 
}


// Steering functions //

// This function will make the battlebot make a 90 degree right turn
void turnRight() {                      
  counterLeft = 0;
  while(counterLeft < 2){
    readRotationLeft();
    left = 255;
    right = 255;
    analogWrite(leftMotorPin2, right);
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
  }
  counterLeft = 0;
  while(counterLeft < 31) {
    readRotationLeft();
    forwardsLeft();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  forwardHalfSquare();
  counterLeft = 0;
}

// This function will make the battlebot make a 90 degree left turn
void turnLeft() {                       
  counterRight = 0;
  while(counterRight < 2){
    readRotationRight();
    left = 255;
    right = 255;
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(rightMotorPin1, LOW);
    analogWrite(rightMotorPin2, right);
  }
  counterRight = 0;
  while(counterRight < 31) {
    readRotationRight();
    forwardsRight();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  forwardHalfSquare();
}

// This function will make the battlebot make a 90 degree left turn after calibrating
void driveInMaze() {                       
  counterRight = 0;
  while(counterRight < 2){
    readRotationRight();
    left = 255;
    right = 255;
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(rightMotorPin1, LOW);
    analogWrite(rightMotorPin2, right);
  }
  counterRight = 0;
  while(counterRight < 31) {
    readRotationRight();
    forwardsRight();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  forwardInMaze();
}

void turnAround() {                 // This function will make the battlebot make a 180 degree turn
  double distanceLeft = 0;
  double distanceRight = 0;
  servoLeft();
  delay(servoDelay);
  detectWall();
  distanceLeft = distance;
  servoRight();
  delay(servoDelay);
  detectWall();
  distanceRight = distance;
  if (distanceLeft < distanceRight){
    servoFront();
    delay(servoDelay);
    counterLeft = 0;
    counterRight = 0;
    while(counterRight < 33) {
      readRotationRight();
      backwardsRight();
    }
    brake();
    counterLeft = 0;
    while(counterLeft < 33) {
      readRotationLeft();
      forwardsLeft();
    }
    brake();
    counterRight = 0;
    counterLeft = 0;
    servoFront();
    brake();
    while(counterRight < 8) {
      readRotationRight();
      driveForward();
    }
    counterRight = 0;
    counterLeft = 0; 
  }
  else {
    servoFront();
    delay(servoDelay);
    counterLeft = 0;
    counterRight = 0;
    while(counterLeft < 33) {
      readRotationLeft();
      backwardsLeft();
    }
    brake();
    counterLeft = 0;
    while(counterRight < 33) {
      readRotationRight();
      forwardsRight();
    }
    brake();
    counterRight = 0;
    counterLeft = 0;
    brake(); 
    servoFront();
    while(counterRight < 8) {
      readRotationRight();
      driveForward();
    }
    counterRight = 0;
    counterLeft = 0;  
  }
}


// Wheel functions //

// This function will make both wheels spin at the maximum speed to prevent one wheel not turning
void calibrateDrive() {                
  forwardLight();
  left = 255;
  right = 255;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW); 
}

// This function will make the left wheel turn backward
void backwardsLeft() {                 
  leftLight();
  left = 220;
  right = 220;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
}

// This function will make the left wheel turn forward
void forwardsLeft() {                   
  rightLight();
  left = 220;
  right = 220;
  analogWrite(leftMotorPin2, right);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
}

void backwardsRight() {                 // This function will make the right wheel turn backward
  rightLight();
  left = 220;
  right = 220;
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin1, right);
  digitalWrite(rightMotorPin2, LOW);
}

// This function will make the right wheel turn forward
void forwardsRight() {                  
  leftLight();
  left = 220;
  right = 220;
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
}


// Rotation Sensor functions //

// This function will read the pulses of the right rotation sensor
void readRotationRight() {              
  rotationStateRight = digitalRead(rightRotationPin);
  if(rotationStateRight != rotationLastStateRight) {
      counterRight++;   
  }
  rotationLastStateRight = rotationStateRight;
  Serial.println(counterRight);
}

// This function will read the pulses of the left rotation sensor
void readRotationLeft() {               
  rotationStateLeft = digitalRead(leftRotationPin);
  if(rotationStateLeft != rotationLastStateLeft) {
      counterLeft++;   
  }
  rotationLastStateLeft = rotationStateLeft;
  Serial.println(counterLeft);
}


// Servo functions //

// This function will make the servo turn to the left 
void servoLeft() {                   
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(2600); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

// This function will make the servo turn to the front
void servoFront() {                  
  // A pulse each 20ms  
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(1650); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

// This function will make the servo turn to the right
void servoRight() {                   
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(600); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

//closes the grabber
void grabberClosed() {
  digitalWrite(grabberPin, HIGH);
  delayMicroseconds(950); // Duration of the pusle in microseconds
  digitalWrite(grabberPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
}

// opens the grabber
void grabberOpen() {
  digitalWrite(grabberPin, HIGH);
  delayMicroseconds(1650); // Duration of the pusle in microseconds
  digitalWrite(grabberPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
}


// Loop function //

// This function will make the battle bot solve the maze
void hugRightWall() {
  if(checking == false) {                                     // Check if the function is already running
    counterLeft = 0;                                          // Reset the left pulse count
    counterRight = 0;                                         // Reset the right pulse count
    checking = true;                                          
    brake();    
    servoRight();                                              // Position the servo to look to the right
    delay(servoDelay);
    detectWall();                                             // Activates the ultra sonic distance sensor and checks if there is a wall
    if(distance < safeTurnDistance) {                                       // If a wall is detected on the right side, position the servo to look to the front
      servoFront();
      delay(servoDelay);
      detectWall();
      if(distance < safeTurnDistance) {                                     // If a wall is detected on the front side, position the servo to the left
        servoLeft();
        delay(servoDelay);
        detectWall(); 
        if(distance < safeTurnDistance) {                                   // If a wall is detected on the left side, the battlebot will turn around
          servoFront();
          delay(servoDelay);
          counterLeft = 0;
          counterRight = 0;
          turnAround();
          checkFinished();
          brake();
          checking = false;
        }
        else {                                                // If no path is detected on the sides, turn to the left
          servoFront();
          turnLeft();
          brake();
          checkFinished();
          checking = false;
        }
      }
      else {                                                 // If a path is detected on the right side, drive one square forward
        servoFront();
        delay(servoDelay);
        forwardOneSquare();
        brake();
        checkFinished();
        checking = false;
      }
    }
    else {                                                  // If a path is detected on the left side, turn to the right
      servoFront();
      delay(servoDelay);
      turnRight();
      brake();
      checkFinished();
      checking = false;
    }
  }
  else {
    checking = false;
  }
}


// Line sensor functions //


void updateLineData()
{
  position = qtr.readLineBlack(sensorValues);
  bool isCurrentlyOnLine = false;
  for (int i = 0; i < 8; i++)
  {
    if (sensorValues[i] > qtr.calibrationOn.maximum[i] - 100)
    {
      isCurrentlyOnLine = true;
      break;
    }
  }
  isOnLine = isCurrentlyOnLine;
}


bool isAllOnLine()
{
  updateLineData();
  bool allSensors = true;
  for (int i = 0; i < 8; i++)
  {
    if (sensorValues[i] < qtr.calibrationOn.maximum[i] - 100)
    {
      allSensors = false;
      break;
    }
  }
  return allSensors;
}

// This function makes the line sensor calibrate
void calibrateSensor()
{
  // First open the grapper, then drive forward over the calibration lines, then proceed to pick up the pawn and rotate towards the course
  grabberOpen(); 
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);
  forward(253,255);
  delay(30);
  forward(120,110);
  while (true)
  {
    qtr.calibrate();
    if (isAllOnLine())
    {
      if (wasOnLines && lastAllLines > millis() - 2000)
      {
        while (true)
        {
          qtr.calibrate();
          if (!isAllOnLine())
          {
            delay(250);
            break;
          }
        }
        break;
      }
      else
      {
        wasOnLines = true;
        lastAllLines = millis();
      }
    }
    else
    {
        wasOnLines = false;
        lastAllLines = 0;
    }
  }
  brake();
  delay(100);
  grabberClosed();
  driveInMaze();
}

// This function activates both motors and will make the battlebot drive forward
void forward(int left, int right) {                   
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);   
}

// Function for checking whether the robot has finished the course
void checkFinished() {
    if (bool currentAllOnline = isAllOnLine()) {
        // If the robot already was on all lines and more than one and a half seconds has passed, the robot is at the end of the course and thus finished
        if (wasAllOnLine && lastAllOnLine > millis() - 1500)
        {
            isFinished = true;
        }
        // If it wasnt yet set the values for the next time we check whether the robot is finished
        else
        {
            wasAllOnLine = true;
            lastAllOnLine = millis();
        }
    }
    // Reset the values incase the robot was on all black but no longer is, without half a second passing
    else
    {
        wasAllOnLine = false;
        lastAllOnLine = 0;
    }
}

// Function for positioning the pawn at the end of the course
void positionPawn() {
    // Open the grapper and drive backwards for 1 second
    forward(0, 0);
    delay(200);
    grabberOpen();
    delay(200);
    forward(-255, -255);
    delay(1000);
    forward(0, 0);
}
