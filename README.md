[![C](https://img.shields.io/badge/gcc-avr--gcc%205.4-informational.svg)](https://gcc.gnu.org)
[![PowerShell](https://img.shields.io/badge/PowerShell-5.1-informational.svg)](https://docs.microsoft.com/en-us/powershell)
[![Bash](https://img.shields.io/badge/Shell-Bash-informational.svg)](https://www.gnu.org/software/bash)
# Smart-Trigger
## An embedded trigger for the [IKEA-IoT-Interface](https://github.com/danielshifman/IKEA-IoT-Interface)

---
### Description
The Smart-Trigger is an embedded device that can be used to trigger the IKEA-IoT-Interface over serial.
The momentary switch toggles the serial connection and is used a soft active/inactive switch (indicated by the yellow LED).
The potentiometer is used to set the threshold for presence detection.

While the device is active, when the distance sensor detects an object within the threshold distance for sufficiently long (indicated by the red LED), the trigger will be "armed" (indicated by the green LED).

While armed, if the microphone detects a loud enough sound, the trigger will be activated and the toggle signal will be sent over serial.

### Hardware
- AVR Microcontroller (I used an ATMega328p)
- HC-SR04 Ultrasonic Distance Sensor
- KY-04 Microphone Module
- Potentiometer (I used a 10kΩ)
- Momentary switch
- 3 LEDs
- 4 Resistors
  - 3 220Ω
  - 1 10kΩ

#### Wiring
The ports used for each component are defined in `main.c`.
If you use different ports, you will need to change the values in the `#define` statements.
[Example schematic for the ATMega328p](smart-trigger-schematic.pdf)

### Flashing
- Be sure to have avr-gcc and avr-libc installed
- After connecting the board to your computer, make a note of the serial port it is connected to
  - On Linux, this will be something like `/dev/ttyACMx`
  - On Windows, this will be something like `COMx`
  - On Mac, this will be something like `/dev/cu.usbmodemxxxx`
- Set the `PORT` variable to this value, then run `make install` in the project directory
- If you wish to set a different clock speed, change the `F_CPU` variable in the Makefile and the `CLOCK_FREQ` variable in the `serial.h` file
  - The default is 16MHz
- If you are using a different board, you may need to change the `mmcu` variable in the Makefile

### Usage
- Connect the board to your computer
- Run the appropriate script for your OS to listen for serial input (set the script variables before running)
    - `serialRequest.ps1` on Windows
    - `serialRequest.sh` on Linux and Mac
- Press the momentary switch to enable serial communication (on is indicated by the yellow LED)
- Arm and trigger the device as desired
  - Ex.1: Hold out your hand to arm, then snap your fingers to trigger
  - Ex.2: Position yourself in front of the device to arm, then clap to trigger
  - Ex.3: Get your head close to arm, then "blow out the light" to trigger
  - Be creative!

### Customisation
In the current state, the device only toggles the "isOn" attribute of the device "Desk Lamp".
Other actions may be written. Feel free to customise to your needs.

**Don't forget to tune the microphone sensitivity with the potentiometer on the microphone module.**