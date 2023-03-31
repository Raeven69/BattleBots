/************************************
***            INF1A              ***
***            29-3-2023          ***
***            Versie 0.9         ***
***            Jannes             ***
************************************/


/************************************
***            Libraries          ***
************************************/
#include <QTRSensors.h>                      // Library used to control the line sensor
#include <Adafruit_NeoPixel.h>               // Library used to control the NeoPixels

QTRSensors qtr; 


/************************************
***            Pin numbers        ***
************************************/
const int grabberPin = 2;                    // Servo for grabbing the pion
const int leftMotorPin1 = 3;                 // Left motor backwards
const int triggerPin = 4;                    // Ultra sonic distance sensor trigger
const int leftMotorPin2 = 5;                 // Left motor forwards
const int rightMotorPin2 = 6;                // Right motor forwards
const int echoPin = 7;                       // Ultra sonic distance sensor echo
const int servoPin = 9;                      // Servo for the ultra sonic distance sensor
const int rightRotationPin = 10;             // Right wheel rotation sensor
const int rightMotorPin1 = 11;               // Right motor backwards
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



/************************************
***            Variables          ***
************************************/
//====Rotation sensor variables====//
// Counts the amount of pulses detected by the rotation sensor
int counterRight = 0;                     
int counterLeft = 0;

// Saves the rotation state
int rotationStateRight;                   
int rotationStateLeft;
int rotationLastStateRight;              
int rotationLastStateLeft;
int leftRotationState;
int leftRotationLastState;
int rightRotationState;
int rightRotationLastState;

//====Wheel speed variables====//
// Speed of the left motor
int left = 255;     
// Speed of the right motor
int right = 255;                         

//====NeoPixel variables====//
// Start value for millis Neopixel
unsigned long timeNe = 0;                 

//====Distance sensor variables====//
// Time detected by the ultra sonic distance sensor
unsigned long duration;    
// Distance in centimetres               
double distance;                          

//====Line sensor variables====//
// Has the battle bot reached the end square
boolean endReached = false;
// Saves the calibrated value
int calibratedValue;
// Should the battlebot calibrate
boolean shouldCalibrate = true;
// Sensor variables
bool outerRight;
bool farRight;
bool SRight;
bool innerRight;
bool innerLeft;
bool SLeft;
bool farLeft;
bool outerLeft;
int calibrationTime = 40;

//====Other variables====//
// Variable to check if the robot is checking for a path 
boolean checking = false;   
// Minimum distance for the battlebot to turn to the left or right                            
const int safeTurnDistance = 25;
// amount of time the servo looks before the code continues
const int servoDelay = 300;
// Check if the battlebot is moving
bool moving = false;
// Check if the battlebot is starting
bool starting = true;



/************************************
***            Setup              ***
************************************/
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
  detectWall();
  // Wait until the distance sensor detects the other battlebot dropping the flag
  while(distance > safeTurnDistance){
    detectWall();
    stopMoving();
  }
  // Wait 2 seconds before starting
  delay(2000);
  // Open the grabber
  grabberOpen();
  // Initialize linesSensor
  qtr.setTypeAnalog();                                    
  qtr.setSensorPins((const uint8_t[]){lineSensorOuterLeft, lineSensorFarLeft, lineSensorLeft, lineSensorInnerLeft, lineSensorInnerRight, lineSensorRight, lineSensorFarRight, lineSensorOuterRight}, 8);  // Initialize linesSensor 
}



/************************************
***            Loop               ***
************************************/
void loop(){
  readSensor();
  if(starting == true){
    start();
    readSensor();
  }
  if(SRight == 1 && innerRight == 1 && innerLeft == 1 && SLeft == 1 && starting == false){
    drive(0,0);                                          // Stop
    delay(100);                                          // Delay
    drive(150,135);
    delay(150);
    drive(0,0);
    delay(70);
    readSensor();

    // Check if it stil sees all black
    if((SRight == 1 && innerRight == 1 && innerLeft == 1 && SLeft == 1 && starting == false)){
       drive(0,0);                                       // Stop
       grabberOpen();                                    // Open gripper
       delay(350);                                       // Delay
       drive(-220, -200);                                // Drive away
       delay(900);                                       // Delay
       endReached = true;                                // Reached the end
       while(endReached){
        drive(0,0);
       }
    }
  }
  else if (innerLeft == 1 && innerRight == 1 && starting == false){
    drive(255,235);
  }
  // If line is slightly centered
  else if (innerLeft == 1 && innerRight == 0 && starting == false){
    drive(240,255);
  }
  // If line is slightly centered
  else if (innerLeft == 0 && innerRight == 1 && starting == false){
    drive(255,205);
  }
  // If line is slightly to the left
  else if (SLeft == 1 && starting == false){
    drive(135,255);
  }
  // If line is slightly to the right
  else if (SRight == 1 && starting == false){
    drive(255,115);
  }
  // If line is to the left
  else if (farLeft == 1 && starting == false){
    drive(0,255);
  }
  // If line is to the right
  else if (farRight == 1 && starting == false){
    drive(255,0);
  }
  // If line is far to the right
  else if (outerRight == 1 && starting == false){
    drive(255,0);
  }
  // If line is far to the left
  else if (outerLeft == 1 && starting == false){
    drive(0,255);
  }
  else{
    hugRightWall();
  }
}




/************************************
***            Functions          ***
************************************/
//====NeoPixel functions====//

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


//====Ultra sonic distance sensor functions====//

// This function activates the ultra sonic distance sensor and it calculates the distance of the object in front of it in centimetres
void detectWall(){                     
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


//====Driving functions====//

// This function makes the battlebot drive one square forward in the maze
void forwardOneSquare(){               
  while(counterRight < 1) {
    readRotationRight();
    calibrateDrive();
  }
  counterRight = 0;
  while(counterRight < 52){
    readRotationRight();
    driveForward();
  }
}

// This function makes the battlebot drive half a square forward in the maze
void forwardHalfSquare(){               
  while(counterRight < 1){
    readRotationRight();
    calibrateDrive();
  }
  counterRight = 0;
  while(counterRight < 29){
    readRotationRight();
    driveStraightForward();
  }
}

void positionAfterTurnAround(){
  while(counterRight < 1) {
    readRotationRight();
    driveForward();
  }
  counterRight = 0;
  while(counterRight < 8){
    readRotationRight();
    driveForward();
  }
  counterRight = 0;
}

// This function activates both motors and will make the battlebot drive forward
void driveForward(){                   
  forwardLight();
  left = 241;
  right = 233;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);  
}

// This function activates both motors and will make the battlebot drive forward while driving for a short amount of time
void driveStraightForward(){                   
  forwardLight();
  left = 242;
  right = 233;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);  
}

// This function deactivates both motors and will make the battlebot stop driving for a short time
void brake(){                          
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
void stopMoving(){                          
  backwardLight();
  left = 0;
  right = 0;
  analogWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW); 
}


//====Steering functions====//

// This function will make the battlebot make a 90 degree right turn
void turnRight(){                      
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
  while(counterLeft < 31){
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
void turnLeft(){                       
  counterRight = 0;
  while(counterRight < 1){
    readRotationRight();
    left = 255;
    right = 255;
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(rightMotorPin1, LOW);
    analogWrite(rightMotorPin2, right);
  }
  counterRight = 0;
  while(counterRight < 31){
    readRotationRight();
    forwardsRight();
  }
  brake();
  counterLeft = 0;
  counterRight = 0;
  forwardHalfSquare();
}

void turnAround(){                 // This function will make the battlebot make a 180 degree turn
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
    while(counterRight < 33){
      readRotationRight();
      backwardsRight();
    }
    brake();
    counterLeft = 0;
    while(counterLeft < 33){
      readRotationLeft();
      forwardsLeft();
    }
    brake();
    counterRight = 0;
    counterLeft = 0;
    servoFront();
    brake();
    positionAfterTurnAround(); 
  }
  else {
    servoFront();
    delay(servoDelay);
    counterLeft = 0;
    counterRight = 0;
    while(counterLeft < 33){
      readRotationLeft();
      backwardsLeft();
    }
    brake();
    counterLeft = 0;
    while(counterRight < 33){
      readRotationRight();
      forwardsRight();
    }
    brake();
    counterRight = 0;
    counterLeft = 0;
    brake(); 
    servoFront();
    positionAfterTurnAround();  
  }
}


//====Wheel functions====//

// This function will make both wheels spin at the maximum speed to prevent one wheel not turning
void calibrateDrive(){                
  forwardLight();
  left = 255;
  right = 255;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW); 
}

// This function will make the left wheel turn backward
void backwardsLeft(){                 
  leftLight();
  left = 220;
  right = 220;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
}

// This function will make the left wheel turn forward
void forwardsLeft(){                   
  rightLight();
  left = 220;
  right = 220;
  analogWrite(leftMotorPin2, right);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(rightMotorPin1, LOW);
}

// This function will make the right wheel turn backward
void backwardsRight(){                 
  rightLight();
  left = 220;
  right = 220;
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  analogWrite(rightMotorPin1, right);
  digitalWrite(rightMotorPin2, LOW);
}

// This function will make the right wheel turn forward
void forwardsRight(){                  
  leftLight();
  left = 220;
  right = 220;
  digitalWrite(leftMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin1, LOW);
  analogWrite(rightMotorPin2, right);
}


//====Rotation Sensor functions====//

// This function will read the pulses of the right rotation sensor
void readRotationRight(){              
  rotationStateRight = digitalRead(rightRotationPin);
  if(rotationStateRight != rotationLastStateRight){
      counterRight++;   
  }
  rotationLastStateRight = rotationStateRight;
  Serial.println(counterRight);
}

// This function will read the pulses of the left rotation sensor
void readRotationLeft(){               
  rotationStateLeft = digitalRead(leftRotationPin);
  if(rotationStateLeft != rotationLastStateLeft){
      counterLeft++;   
  }
  rotationLastStateLeft = rotationStateLeft;
  Serial.println(counterLeft);
}


//====Servo functions====//

// This function will make the servo turn to the left 
void servoLeft(){                   
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(2600); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

// This function will make the servo turn to the front
void servoFront(){                  
  // A pulse each 20ms  
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(1650); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

// This function will make the servo turn to the right
void servoRight(){                   
  // A pulse each 20ms
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(600); // Duration of the pusle in microseconds
  digitalWrite(servoPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
  // Pulses duration: 600 - 0deg; 1650 - 90deg; 2600 - 180deg
}

//closes the grabber
void grabberClosed(){
  digitalWrite(grabberPin, HIGH);
  delayMicroseconds(950); // Duration of the pusle in microseconds
  digitalWrite(grabberPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
}

// opens the grabber
void grabberOpen(){
  digitalWrite(grabberPin, HIGH);
  delayMicroseconds(1650); // Duration of the pusle in microseconds
  digitalWrite(grabberPin, LOW);
  delayMicroseconds(18550); // 20ms - duration of the pusle
}


//====Loop function====//

// This function will make the battle bot solve the maze
void hugRightWall(){
  // Check if the function is already running
  if(checking == false){             
    // Reset the left pulse count                        
    counterLeft = 0;                                          
    // Reset the right pulse count
    counterRight = 0;                                         
    checking = true;                                          
    brake();    
    // Position the servo to look to the right
    servoRight();                                              
    delay(servoDelay);
    // Activates the ultra sonic distance sensor and checks if there is a wall
    detectWall();                                             
    // If a wall is detected on the right side, position the servo to look to the front
    if(distance < safeTurnDistance){                                       
      servoFront();
      delay(servoDelay);
      detectWall();
      // If a wall is detected on the front side, position the servo to the left
      if(distance < safeTurnDistance){                                     
        servoLeft();
        delay(servoDelay);
        detectWall(); 
        // If a wall is detected on the left side, the battlebot will turn around
        if(distance < safeTurnDistance){                                   
          servoFront();
          delay(servoDelay);
          counterLeft = 0;
          counterRight = 0;
          turnAround(); 
          brake();
          checking = false;
        }
        // If no path is detected on the sides, turn to the left
        else{                                                
          servoFront();
          turnLeft();
          brake();
          checking = false;
        }
      } 
      // If a path is detected on the right side, drive one square forward
      else{                                                
        servoFront();
        delay(servoDelay);
        forwardOneSquare();
        brake();
        checking = false;
      }
    }
    // If a path is detected on the left side, turn to the right
    else{                                                  
      servoFront();
      delay(servoDelay);
      turnRight();
      brake();
      checking = false;
    }
  }
  else{
    checking = false;
  }
}


//====Line sensor functions====//


// This function activates both motors and will make the battlebot drive forward
void drive(int left, int right){
  if(moving == false && left < 150 && left > 0 && right < 150 && right > 0){
    analogWrite(rightMotorPin1, 155);
    analogWrite(leftMotorPin2, 155);
    delay(15);
    moving = true;
  }
  if(left < 0){
      analogWrite(leftMotorPin2, 0);
      analogWrite(leftMotorPin1, abs(left));
  } 
  else{
      analogWrite(leftMotorPin2, left);
      analogWrite(leftMotorPin1, 0);
  }
  if(right < 0){
      analogWrite(rightMotorPin2, 0);
      analogWrite(rightMotorPin1, abs(right));
  } 
  else{
      analogWrite(rightMotorPin2, right);
      analogWrite(rightMotorPin1, 0);
  }
  if(right == 0 && left == 0){
    moving = false;
  }
}

// Get Sensitivity from lineSensor
int getLineSensorSensitivity(int margin = 80){           
    int totalSize = 0;
    for (int i = 0; i < 8; i++){
        totalSize += qtr.calibrationOn.maximum[i];
    }
    return (int)(totalSize / 8 - margin);
}

void start(){
  if(shouldCalibrate)
  {
    int i;
    for (i = 0; i < calibrationTime; i++)
    {
      drive(255,255);
      delay(10);
      drive(119,110);
      qtr.calibrate();
      delay(30);
    }
    Serial.println("");
    Serial.println("Calibration complete");
    calibratedValue = getLineSensorSensitivity();
    Serial.println(calibratedValue);
  }
  readSensor();
  Serial.println("Sensor: " + innerLeft);
  if(SLeft == 0 && innerLeft == 0 && innerRight == 0 && SRight == 0){     
  // End of the black surface is reached
    delay(50);
    drive(0,0);
    grabberClosed();
    delay(300);
    drive(-200, -200);
    delay(250);
    rotate(-89);
    starting = false;
  }
  delay(250);
  readSensor();
}

void readSensor(){
  outerRight = analogRead(lineSensorOuterRight) > calibratedValue;
  farRight = analogRead(lineSensorFarRight) > calibratedValue;
  SRight = analogRead(lineSensorRight) > calibratedValue;
  innerRight = analogRead(lineSensorInnerRight) > calibratedValue;
  innerLeft = analogRead(lineSensorInnerLeft) > calibratedValue;
  SLeft = analogRead(lineSensorLeft) > calibratedValue;
  farLeft = analogRead(lineSensorFarLeft) > calibratedValue;
  outerLeft = analogRead(lineSensorOuterLeft) > calibratedValue;
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
