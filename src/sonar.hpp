const int echoPin = 4;
const int triggerPin = 7;

void initSonar() {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

bool isBlocked(int distance = 750) {
    digitalWrite(triggerPin, HIGH);
    digitalWrite(triggerPin, LOW);
    return pulseIn(echoPin, HIGH) < distance;
}