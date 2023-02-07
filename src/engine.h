const int leftMotorPin1 = 5;
const int leftMotorPin2 = 6;
const int rightMotorPin2 = 10;
const int rightMotorPin1 = 11;

bool isDriving = false;

void initEngine() {
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);
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