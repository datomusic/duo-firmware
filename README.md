# Dato DUO
The Dato DUO is a musical instrument that is designed to be played by two people. 

The firmware is open source, although changing it is not recommended for beginners. Steps below are tested on Mac OS X. We're welcoming contributors who want to help us get this working on other platforms.

## Firmware/software
The electronic heart of the Dato DUO is the NXP Kinetis K20 microcontroller, the same as used in the [Teensy 3.2](https://www.pjrc.com/teensy/). Audio is generated using the [Audio Library](https://github.com/PaulStoffregen/Audio).

### Installing
For now, it's not possible to upload firmware using the Arduino IDE. You will need to download and install it, though, in order to get all required tools and libraries. Advanced users can use the supplied makefile.

1. Download and install the Arduino IDE from [Arduino.cc](https://www.arduino.cc/en/Main/Software)

2. Download and install Teensyduino from [PJRC.com](https://www.pjrc.com/teensy/teensyduino.html). In the install wizard, enable the Audio, FastLED and Keypad libraries
![Teensyduino installer with Audio library checked](/img/teensyduino-installer-audio-library.png?raw=true)

3. From a command line window, clone this repository including submodules `git clone --recursive https://github.com/datomusic/duo-firmware`

### Compiling and uploading
Go to the duo-firmware folder and type `make`.

Connect the Dato DUO to your computer using a micro USB cable. Put your Dato DUO into firmware update mode by gently pressing a pointy object into the little hole next to the headphone output. The lights will stop flashing and the Play button will become blue (early DUO's with a serial number below 350 will not show a blue button).

On your computer, type `make dfu` to upload the firmware to the DUO.

### Troubleshooting
If you cloned this repository without the submodules, go to the duo-firmware folder and type `git submodule update --init --recursive` to fetch all required submodules.

If that doesn't help, please file a GitHub issue with as much detail as you can so that we can troubleshoot the issue.


## Where everything came from

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
