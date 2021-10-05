# Overview

The code here is meant for the Raspberry Pi Pico microcontroller. It allows for 2 Sega Genesis
controllers to be plugged in and interface with any USB host device.

# Hardware

Refer to schematic.png for the recommended electronics setup.

# Configuration

Refer to configuration.h for settings that may be modified to configure this however you'd like.
Players 1 and 2 may be set as a keyboard or gamepad devices (gamepad by default). Mapped keys may
also be configured there.

When USB_ALWAYS_CONNECTED is true, all controllers are always configured and connected to USB when a
host is present.

When USB_ALWAYS_CONNECTED is set to false, USB will stay disconnected until a controller for player
1 is detected, and then will remain connected until player 1 is removed. The USB descriptor will
automatically update for 1 or 2 players as player 2 is removed or connected. When the USB descriptor
needs to be updated, USB will disconnect for a short while in order to force the update with host.
When player 1 is removed or player 2 is removed while player 1 is connected, the USB interface will
disconnect after the delay of DISCONNECT_DELAY_S. This was all done as a workaround for retropie 
purposes to reduce confusion and achieve the following.
- When no controllers are connected, this device will completely detach from USB, forcing retropie
to choose other connected USB or Bluetooth controllers.
- Retropie doesn't do a good job of predictably enumerating 2 controllers under the same USB config. 
When there is only a controller connected for player 1, only 1 player will be configured over USB.
If player 2 plugs in, there is still a possibility of the two controllers enumerating incorrectly.
The disconnect delay allows for the user to manually swap controllers without causing 
disconnect/reconnect events.

# Build

## Prerequisites

### The SDK

The pico SDK is linked for completeness so that the version that this is compiled and tested with is
documented. You may use your own downloaded version of the SDK without any modifications as long as
your PICO_SDK_PATH environment variable is set.

To use the local copy of the SDK, simply execute `git submodule update --recursive --init`

### CMAKE and Compiler

Ensure cmake and compiler for pico is installed: `sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential`

### VSCode

Install vscode: `sudo apt install code`

Execute vscode: `cd <project_dir>; code .`

Within vscode, install the extensions `C/C++`, `CMake`, and `CMake Tools`.

## Compiling the Project

Open the project in code: `cd <project_dir>; code .`

Open the CMake extension in vscode and build GenesisControllerPico. Use gcc-arm-none as your compiler. If that doesn't show up in the list that pops up, select to search for compatible compilers, and then try again.

# External Links

Getting started with the Pico SDK:
https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf

Youtube video demonstration of this project:
https://www.youtube.com/watch?v=Fbt-3-cg1RE&t=95s

3D printed case on thingiverse (50mm x 70mm dev board and DB9 connectors required):
https://www.thingiverse.com/thing:4761232

Great resources on the Sega Genesis controller:
https://www.raspberryfield.life/2019/03/25/sega-mega-drive-genesis-6-button-xyz-controller/
https://jonthysell.com/2014/07/26/reading-sega-genesis-controllers-with-arduino/
