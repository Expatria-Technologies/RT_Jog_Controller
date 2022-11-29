![Logo](/readme_images/logo_sm.jpg)
# Jog2K Jog Controller for GRBLHAL2000 and Flexi-HAL
<img src="/readme_images/IMG_0091.jpg" width="800">

Expatria Technologies jog controller for GRBLHAL2000 and Flexi-HAL boards

Assembled PCBS currently available in our online store:

https://expatria.myshopify.com/products/jog2k-keypad-pendant-pcba

Please consider buying a board to support our open-source designs.

This jog controller was developed for the GRLBHAL20000 and Flexi-HAL boards.  It is designed to be relatively inexpensive and simple to assemble.  It needs minimal manual wiring and leverages 3d printable parts for assembly.  The system consists of a small interface module that is installed on the QWIIC/I2C headers on the Hal2000, plus a simple lighted button keypad that uses a Raspberry Pi Pico to send character commands to GRBLHAL and receive status information.  This pendant relies on the modified I2C Keypad plugin that was extended by Expatria Technologies:

https://github.com/Expatria-Technologies/Plugin_I2C_keypad

The key features of the Jog2K:

1) Immediate access to GRBLHAL jogging functionality and real-time commands
2) DRO with selectable WCS
3) Alternate button function macros

## Usage

<img src="/readme_images/buttonmap.jpg" width="400">

Many of the buttons on the Jog2K have alternate functions that are accessed via the SHIFT/FUNCTION button.  The XYZ jog buttons give access to the configurable macros.  In the event that a 4th axis is enabled in GRBLHAL, the macro functions of the Z axis buttons are lost and the alternate function for those buttons changes to jogging the A axis.

The alternate fuctions for the Mist and Flood buttons control the current jog mode and rate.  The Shift-Mist button toggles between Fast, Slow and step modes (these are configurable in IOSender).  The Shift-Flood button will change the decimal place of the current jog mode.

The Shift-HOME function will toggle between the active WCS: G54, G55 etc.

The HALT button has an alternate function which is to flip the OLED screen vertically.  There is no standard orientation for these screens so if yours is upside-down after installation, simply activate the screen flip.  The screen orientation is stored in NVM.

## Macros

It is easiest to configure the macros in IOSender.

<img src="/readme_images/macros.jpg" width="400">

Each macro is limited to 127 characters.  The macro on the spindle button has a special function.  When the spindle is off, the macro will run and it is intended to start up the spindle at a slow rate (the default value for this macro is 200 RPM).  When the spindle is running, activating the spindle macro button will send an M5 command to stop the spindle.  The primary use of this functionality is for manual edge-finding.

## Cable Interface - 5 Gbps Link Cable Required. Avoid E-Marker

Some pre-release Jog2K systems used RJ45 connections, but all current and future releases will use USB-C connectors on both the Jog2K and the host interface module.  It is cricitical that the correct USBC cable is used.  Many USB-C cables do not have all of the conductors inside and these will not work with the Jog2k.  Always use a cable that is rated for USB-C display (5 Gbps data transfer).  An Oculus Link cable is commonly available and works well.  These cables also have the advantage that they are usually high quality and well shielded.  Newer cables (Gen 2) that advertise power delivery will not work.


## Assembly

Upon receipt of the Jogger, the small interface module should be snapped off from the main PCB.  This module is installed on the 5 and 4 pin QWIIC/I2C headers on the GRBLHAL2000 or Flexi-HAL board.  The reason the module is included with the Jog2K is due to part availability challenges.  Different interface designs may be used as parts go in and out of stock, and therefore the interface module is included to ensure compatibility.

Thank you to [Jaymis](https://jaymis.com/) from the PrintNC Discord for this awesome video showing some great soldering technique.

[![Jog2K](https://img.youtube.com/vi/nImVRGaTyQw/0.jpg)](https://www.youtube.com/watch?v=nImVRGaTyQw)

In addition to the interface module, the Jog2K requires a 0.96 or 1.3 inch I2C OLED screen.  The current A6 revision of the board uses a discrete RP2040 MCU.  If you wish to use the older board based on the Pi Pico, the A5 files are archived.

<img src="/readme_images/bootsel_location.jpg" width="400">


Ensure that your oled screen pinout matches the pinout labelled on the jogger PCBA.

<img src="/readme_images/screen_pinout.jpg" width="300">

Install the OLED in the 4 pin header location and try to keep it straight when soldering.

<img src="/readme_images/screen_install.jpg" width="400">

Once the PCBA is fully assembled simply print the buttons in clear PETG so that the neopixel functions are visible.  The shift, coolant and override buttons are not backlit and can be printed in black or other suitable colors. For these functions, there are lightpipe models provided.  Finally there are two smaller buttons for BOOTSEL and RESET.

A window for the screen will need to be cut on your CNC from clear acrylic or polycarbonate.

For current and future USB-C equipped Jog2K boards, the jog_bottom_box_slim should be used.  The previous model is included only for reference.

<img src="/readme_images/unit_photo.jpg" width="400">

## Screen
Originally the Jog2K was designed to be used with a 0.96 inch OLED screen that was mounted to the PCB.  However, thanks to the PrintNC community, a new 3d printed enclsoure was developed that allows for the use of a 1.3 inch OLED that is mounted directly the the front panel.  This results in a significantly more readable display and is a massive upgrade over the original screen.  However, this means that some manual assemlby and soldering is required.  Updated STLs are in the Jog2K Deluxe Edition folder in this repo.

### Attributions
This project uses components from the very helpful actiBMS library for JLCPCB SMT parts.

https://github.com/actiBMS/JLCSMT_LIB
