/************************************
***         Sonar sensor          ***
************************************/

// Sonar pins
const int echoPin = 4;
const int triggerPin = 7;

// Initialize the sonar pins
void initSonar(){
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

// Check whether there is an obstacle in front of the robot
bool isBlocked(int distance = 750){
    digitalWrite(triggerPin, HIGH);
    digitalWrite(triggerPin, LOW);
    return pulseIn(echoPin, HIGH) < distance;
}