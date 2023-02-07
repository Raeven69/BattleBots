const int driveButtonBin = 3;

const int buttons[1] = {driveButtonBin};
long nextClick = millis() + 500;

void initButtons() {
    for (int i = 0; i < (int)(sizeof(buttons) / sizeof(int)); i++) {
        pinMode(buttons[i], INPUT);
    }
}

bool isButtonPressed(int buttonIndex = 0) {
    if (digitalRead(buttons[buttonIndex]) == 0 && millis() > nextClick) {
        nextClick = millis() + 500;
        return true;
    }
    return false;
}