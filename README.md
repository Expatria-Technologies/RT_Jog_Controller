# Jog2K I2C Jog Controller for GRBLHAL2000

![Logo](/readme_images/logo_sm.jpg)

Expatria Technologies I2C jog controller for GRBLHAL2000 boards

This jog controller was developed for the GRLBHAL20000 board.  It is designed to be relatively inexpensive and simple to assemble.  It does not rely on any manual wiring focuses on 3d printable parts for assembly.  The system consists of a small interface module that is installed on the QWIIC/I2C headers on the Hal2000, plus a simple lighted button keypad that uses a Raspberry Pi Pico to send character commands to GRBLHAL and receive status information.  This pendant relies on the modified I2C Keypad plugin that was extended by Expatria Technologies:

https://github.com/Expatria-Technologies/Plugin_I2C_keypad

The key features of the Jog2K:

1) Immediate access to GRBLHAL jogging functionality and real-time commands
2) DRO with selectable WCS
3) Simple macro functionaltiy

## Usage

<img src="/readme_images/buttonmap.jpg" width="500">

Many of the buttons on the Jog2K have alternate functions that are accessed via the SHIFT/FUNCTION button.  The XYZ jog buttons give access to the configurable macros.  In the event that a 4th axis is enabled in GRBLHAL, the macro functions of the Z axis buttons are lost and the alternate function for those buttons changes to jogging the A axis.

Other than the jog buttons, the HALT button has an alternate function which is to flip the OLED screen vertically.  There is no standard orientation for these screens so if yours is upside-down after installation, simply activate the screen flip.  The screen orientation is stored in NVM.

## Macros

It is easiest to configure the macros in IOSender.

<img src="/readme_images/macros.jpg" width="500">

Each macro is limited to 127 characters.  The macro on the spindle button has a special function.  When the spindle is off, the macro will run and it is intended to start up the spindle at a slow rate (the default value for this macro is 200 RPM).  When the spindle is running, activating the spindle macro button will send an M5 command to stop the spindle.  The primary use of this functionality is for manual edge-finding.

## Cable Interface - 5 Gbps Link Cable Required.

some pre-release Jog2K systems used RJ45 connections, but all current and future releases will use USB-C connectors on both the Jog2K and the host interface module.  It is cricitical that the correct USBC cable is used.  Many USB-C cables do not have all of the conductors inside and these will not work with the Jog2k.  Always use a cable that is rated for USB-C display (5 Gbps data transfer) and 3A maximum current.  An Oculus Link cable is commonly available and works well.  These cables also have the advantage that they are usually high quality and well shielded.


## Assembly

Upon receipt of the Jogger, the small interface module should be snapped off from the main PCB.  This module is installed on the 5 and 4 pin QWIIC/I2C headers on the GRBLHAL2000 board.  The reason the module is included with the Jog2K is due to part availability challenges.  Different interface designs may be used as parts go in and out of stock, and therefore the interface module is included to ensure compatibility.

In addition to the interfae module, the Jog2K requires a 0.93 or 0.96 inch I2C OLED screen and a Raspberry Pi Pico.  The Pico is installed on the bottom side of the boardby soldering directly to the SMD pads, no headers are used.  The interior pico pads do not need to be soldered from the bottom side.  Once the Pico is installed, flip the board over and apply a significant amount of solder and heat to the BOOTSEL hole on the top side.  The connects the boot select button to the Pico so that the firmware can easily be upgraded after the Jog2k is fully assembled.

<img src="/readme_images/bootsel_location.jpg" width="500">


Ensure that your oled screen pinout matches the pinout labelled on the jogger PCBA.

<img src="/readme_images/screen_pinout.jpg" width="500">

Install the OLED in the 4 pin header location and try to keep it straight when soldering.

<img src="/readme_images/screen_install.jpg" width="500">

Once the PCBA is fully assembled simply print the buttons in clear PETG so that the neopixel functions are visible.  The shift, coolant and override buttons are not backlit and can be printed in black or other suitable colors. For these functions, there are lightpipe models provided.  Finally there are two smaller buttons for BOOTSEL and RESET.

A window for the screen will need to be cut on your CNC from clear acrylic or polycarbonate.

For current and future USB-C equipped Jog2K boards, the jog_bottom_box_slim should be used.  The previous model is included only for reference.

<img src="/readme_images/unit_photo.jpg" width="500">

### Attributions
This project uses components from the very helpful actiBMS library for JLCPCB SMT parts.

https://github.com/actiBMS/JLCSMT_LIB