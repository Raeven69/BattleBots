//Pinnumbers
const int grabberPin = 2;
const int leftMotorPin1 = 3;
const int triggerPin = 4;
const int leftMotorPin2 = 5;
const int rightMotorPin2 = 6;
const int echoPin = 7;
const int rightMotorPin1 = 9;
const int leftRotationSensor = A0;
const int rightRotationSensor = A7;



//Variables
const int turn = 525;
const int around = 1025;
int left = 255;
int right = 255;
unsigned long duration;
unsigned int distance;
bool checkedLeft = false;
bool checkedFrontL = false;
bool checkedLeftL = false;
bool checkedRightL = false;
bool checkingLeft = false;



//Setup
void setup() {
  pinMode(triggerPin, OUTPUT);        //Ultra sonic distance sensor trigger
  pinMode(echoPin, INPUT);            //Ultra sonic distance sensor echo
  Serial.begin(9600);                 //Serial.begin for testing purposes
  pinMode(rightMotorPin1, OUTPUT);    //Right motor
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);     //Left motor
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightRotationPin, INPUT);   //Right rotation sensor
  pinMode(leftRotationPin, INPUT);    //Left rotation sensor
}



//Loop
void loop() {
  detectWall();
  if(checkingLeft == false && distance < 8) {
    Serial.println("Checking left side");
    checkingLeft = true;
    brake();
    turnLeft();
    brake();
    driveForward();
    detectWall();
    while(checkedFrontL == false) {
      Serial.println("Checking Front");
      driveForward();
      detectWall();
      if(distance < 8) {
        Serial.println("Front wall detected");
        brake();
        turnLeft();
        brake();
        checkedFrontL = true;
      }
    }
    if(checkedFrontL == true) {     
      while(checkedLeftL == false) {
        Serial.println("Checking left");  
        driveForward();
        detectWall();  
        if(distance < 8) {
          Serial.println("Left wall detected");
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
        driveForward();
        detectWall();  
        if(distance < 8) {
          Serial.println("Right wall detected");
          brake();
          turnAround();
          brake();
          checkedRightL = true;
        }
      }
    }
  }
  else {
    driveForward();
  }
}



//Functions
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

void stop() {                          //This function deactivates both motors and will make the battlebot stop driving
   left = 0;
   right = 0;
   analogWrite(leftMotorPin2, left);
   digitalWrite(leftMotorPin1, LOW);
   analogWrite(rightMotorPin2, right);
   digitalWrite(rightMotorPin1, LOW); 
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

void checkLeft() {                      //This function will make the battlebot check the left side to see if its the right path to take

}
