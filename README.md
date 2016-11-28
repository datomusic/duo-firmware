#Dato DUO
The Dato DUO is a musical instrument that is designed to be played by two people.

##Firmware/software
The electronic heart of the Dato DUO is the NXP Kinetis K20 microcontroller, the same as used in the [Teensy 3.2](https://www.pjrc.com/teensy/). Audio is generated using the [Audio Library](https://github.com/PaulStoffregen/Audio).

###Installing
For beginners, we recommend using the Arduino IDE for modifying and uploading the firmware. Advanced users can use the supplied makefile, but following the steps below is necessary in both cases.

1. Download and install the Arduino IDE from [Arduino.cc](https://www.arduino.cc/en/Main/Software)

2. Download and install Teensyduino from [PJRC.com](https://www.pjrc.com/teensy/teensyduino.html). In the install wizard, enable the Audio, FastLED and Keypad libraries
![Teensyduino installer with Audio library checked](/img/teensyduino-installer-audio-library.png?raw=true)

3. Clone this repository including submodules `git clone --recursive https://github.com/datomusic/duo-firmware`

###Troubleshooting
If you're getting compilation errors related to missing libraries, make sure you have copied all libraries from the duo-firmware/libraries directory to your Arduino/libraries directory. Please file a GitHub issue with as much detail as you can so that we can troubleshoot the issue.

###Connecting and uploading
Connect the Dato DUO to your computer using a micro USB cable.

Arduino IDE:
- Pick the correct board by choosing `Tools` -> `Boards` -> `Teensy 3.2 / 3.1`
- Set the clock speed to 72 MHz from `Tools` -> `CPU Speed` -> `72 MHz optimized`
- Open the .ino file in the firmware/src folder
- Press upload and wait until compilation finishes. After this, Teensyduino will automatically upload the code to your Dato DUO.

Makefile method:
```
make clean; make upload
```
Currently, this is only tested on Mac.

##Hardware
Circuit schematics will be added to this repository once the final hardware is available.

##Where everything came from

- The `teensy3` sub-folder is taken from [The Teensy 3 Cores](https://github.com/PaulStoffregen/cores/tree/master/teensy3)
- The `tools` sub-folder is taken from [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
- The `src/main.cpp` file is moved, unmodified from `teensy3/main.cpp`
- The `Makefile` file is moved, modified from `teensy3/Makefile`
- The `49-teensy.rules` file is taken from [PJRC's udev rules](http://www.pjrc.com/teensy/49-teensy.rules)

Modifications to `Makefile` include
- Add support for arduino libraries
- Change tools directory
- Calculate target name from current directory
- Prettify rule output
- Do not upload by default, only build
