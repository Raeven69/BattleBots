//Libraries
#include <SoftwareSerial.h>
#include <Servo.h>



//Pinnumbers
SoftwareSerial bluetooth(13, 8); // RX | TX  // Define the pins for the bluetooth module
Servo grabber;                               // Define the name of the servo
const int grabberPin = 9;                   // Servo 
const int leftMotorPin1 = 3;                 // Left motor backwards
const int triggerPin = 4;                    // Ultra sonic distance sensor trigger
const int leftMotorPin2 = 5;                 // Left motor forwards
const int rightMotorPin2 = 6;                // Right motor forwards
const int echoPin = 7;                       // Ultra sonic distance sensor echo
const int rightMotorPin1 = 11;                // Right motor backwards
const int rightRotationSensor = 10;          // Right wheel rotation sensor
const int leftRotationSensor = 12;           // Left wheel rotation sensor



//Variables
const int turn = 525;
const int around = 1025;
int left = 255;                       // Speed of the left motor
int right = 255;                      // Speed of the right motor
unsigned long duration;               // Time detected by the ultra sonic distance sensor
unsigned int distance;                // Distance in centimetres
bool checkedLeft = false;
bool checkedFrontL = false;
bool checkedLeftL = false;
bool checkedRightL = false;
bool checkingLeft = false;
bool checkingSides = false;



//Setup
void setup() {
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
  pinMode(rightRotationSensor, INPUT);   
  pinMode(leftRotationSensor, INPUT);   
}



//Loop
void loop() {
  enableBluetooth();
  detectWall();
  if(distance < 15 && checkingSides == false) {
    brake();
    Serial.println("Wall detected");
    checkingSides = true;
    brake();
    grabberLeft();
    detectWall();
    if(distance < 15) {
      Serial.println("Left wall detected");
      grabberRight();
      detectWall();
      if(distance < 15) {
        Serial.println("Right wall detected");
        Serial.println("No path found, turning around");
        turnAround();
        brake();
        checkingSides = false;
      }
      else {
        Serial.println("Path found on the right side, turning right");
        turnRight();
        brake();
        checkingSides = false;
      }
    }
    else {
      Serial.println("Path found on the left side, turning left");
      turnLeft();
      brake();
      checkingSides = false;
    }
  }
  else {
    driveForward();
    grabberFront();
  }
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
   delay(100);
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
  left = 200;
  right = 210;
  analogWrite(leftMotorPin2, left);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(rightMotorPin1, right);
  delay(turn);
}

void turnLeft() {                       // This function will make the battlebot make a 90 degree left turn
  left = 200;
  right = 210;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  delay(turn);
}

void turnAround() {                     // This function will make the battlebot make a 180 degree turn
  left = 200;
  right = 210;
  digitalWrite(leftMotorPin2, LOW);
  analogWrite(leftMotorPin1, left);
  analogWrite(rightMotorPin2, right);
  digitalWrite(rightMotorPin1, LOW);
  delay(around);
}

void checkLeft() {                      // This function will make the battlebot check the left side to see if its the right path to take
  Serial.println("Checking left side");
  bluetooth.println("Checking left side");
  checkingLeft = true;
  brake();
  turnLeft();
  brake();
  driveForward();
  detectWall();
  while(checkedFrontL == false) {
    Serial.println("Checking Front");
    bluetooth.println("Checking Front");
    driveForward();
    detectWall();
    if(distance < 8) {
      Serial.println("Front wall detected");
      bluetooth.println("Front wall detected");
      brake();
      turnLeft();
      brake();
      checkedFrontL = true;
    }
  }
  if(checkedFrontL == true) {     
    while(checkedLeftL == false) {
      Serial.println("Checking left"); 
      bluetooth.println("Checking left");  
      driveForward();
      detectWall();  
      if(distance < 8) {
        Serial.println("Left wall detected");
        bluetooth.println("Left wall detected");
        brake();
        turnAround();
        brake();
        checkedLeftL = true;
      }
    }
  }
  if(checkedLeftL == true) {     
    while(checkedRightL == false) {
      Serial.println("Checking right");  
      bluetooth.println("Checking right"); 
      driveForward();
      detectWall();  
      if(distance < 8) {
        Serial.println("Right wall detected");
        bluetooth.println("Right wall detected");
        brake();
        turnAround();
        brake();
        checkedRightL = true;
      }
    }
  }  
}

void grabberLeft() {
  grabber.write(180);
  delay(600);
}

void grabberFront() {
  grabber.write(110);
  delay(600);
}

void grabberRight() {
  grabber.write(20);
  delay(600);
}
