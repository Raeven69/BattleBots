const int leftMotorPin1 = 5;
const int leftMotorPin2 = 6;
const int rightMotorPin2 = 3;
const int rightMotorPin1 = 11;
const int leftRotationPin = 12;
const int rightRotationPin = 10;

int leftRotationState;
int leftRotationLastState;
int rightRotationState;
int rightRotationLastState;

bool isDriving = false;

void initEngine() {
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);
    pinMode(leftRotationPin, INPUT);
    pinMode(rightRotationPin, INPUT);
}

void drive(int left, int right) {
    if (left < 0) {
        analogWrite(leftMotorPin1, 0);
        analogWrite(leftMotorPin2, abs(left));
    } else {
        analogWrite(leftMotorPin1, left);
        analogWrite(leftMotorPin2, 0);
    }
    if (right < 0) {
        analogWrite(rightMotorPin1, 0);
        analogWrite(rightMotorPin2, abs(right));
    } else {
        analogWrite(rightMotorPin1, right);
        analogWrite(rightMotorPin2, 0);
    }
    isDriving = abs(left) + abs(right) > 0;
}

void rotate(int rotation) {
    int counter = 0;
    while (counter < (int)((135.0/360.0) * abs(rotation))) {
        if (rotation > 0) {
            drive(150, -150);
        } else {
            drive(-150, 150);
        }
        leftRotationState = digitalRead(leftRotationPin);
        rightRotationState = digitalRead(rightRotationPin);
        if (leftRotationState != leftRotationLastState || rightRotationState != rightRotationLastState) {
            counter++;
        }
        leftRotationLastState = leftRotationState;
        rightRotationLastState = rightRotationState;
    }
    drive(0, 0);
}