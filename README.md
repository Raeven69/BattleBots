# BeunBot
Code for three tiny robots that are dikke beun.

## Requirements
- Arduino IDE or Visual Studio Code with the Arduino extension.
- [The Servo Library](https://github.com/arduino-libraries/Servo)
- [The QTRSensors Library](https://github.com/pololu/qtr-sensors-arduino)

## Installation
1. Select the branch for which BattleBot you want to use.
2. Clone the repository
    ```
    git clone https://github.com/Raeven69/BattleBots/tree/{branch}
    ```
3. Navigate to the folder
    ```
    cd BattleBots-{branch}
    ```
4.  - If you're using Visual Studio Code with the Arduino extension, simply create a folder called ".vscode" and move the arduino.json file into the newly created folder. Make sure to select your serial port in the bottom right corner before uploading.
    - If you're using the Arduino IDE, open the tools dropdown and set board to Arduino NANO, processor to ATmega328P (For BeunBot use the old bootloader) and make sure that the programmer is set to AVRISP mkII. Also make sure to select your port before uploading.
5. You're ready to go.

## Contributing
Pull requests are welcome, merges will need to be discussed.
If you need help or want to report bugs feel free to open an issue.

## License
MIT License

Copyright (c) 2023 INFA-1A

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.