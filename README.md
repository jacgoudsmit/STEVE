# STEVE: Static Type-safe EVE control library #

Brief list of features:

* Licensed under MIT license; free to use in hobby projects as well as commercial projects (but you must reproduce the copyright notice from the LICENSE file).
* Hardware-specific data is stored statically (not as #defines). It's even possible to control multiple screens at once. 
* Constants are implemented as enum types as much as possible (not as #defines). This drastically reduces the chance of programming errors that are difficult to analyze.
* Hardware Abstraction Layer separates SPI communication from EVE functionality. This makes it easy to port STEVE to other platforms or utilize optimized code for particular purposes.
* Supports most EVE chips: FT810, FT811, FT812, FT813, BT815, BT816, BT817, BT818 (FT800/FT801 not supported).
* Supports capacitive or resistive touch screens.
* Supports audio.

## Introduction ##

The STEVE class implements functionality to control one or more EVE-based LCD displays, with or without (capacitive or resistive) touch screen. EVE stands for Embedded Video Engine. EVE was first developed by FTDI (well known for its USB-to-serial interface chips), but later on, BridgeTek Pte Ltd. took over development and distribution. This is why the first chip type numbers started with FT (FTDI) and later chips start with BT (Bridgetek).

This library is different from other EVE control libraries because it minimizes the use of the preprocessor of the C++ compiler to deal with deals with variations between LCD panels parameters. The variations are implemented in code, not in #defines. This makes it possible to use the same binary code for multiple displays without recompiling. Even multiple dissimilar displays can be controlled at the same time by the same program. This is the reason for the "S" ("Static") in "Steve".

Another difference is that even though this header file uses macros extensively, it uses enum types instead of #define's for the many "magic numbers" that are used to control EVE chips. That way, it forces (or at least encourages) developers to use the appropriate types and values for operations, and helps to avoid bugs that may be difficult to find otherwise. This is the reason for the "T" ("Type-safe") in "Steve".

All the hardware control such as toggling the !CS and !PD chip, handling interrupts, and sending and receiving data over the SPI/QSPI bus, is handled by the SteveHAL class. The library was originally developed for the Arduino platform, but should be easy to port to other platforms by writing a subclass of SteveHAL. Some optimizations are possible; for example if there is a more efficient way for the host to send or receive multiple bytes, all you need is to override the necessary virtual functions.

All the parameters that are display-specific are stored in an instance of the SteveDisplay struct. This should make it easy to write and reuse software that can work on multiple displays, even if they have different major parameters such as width and height.

## Documentation references ##

Data sheets for the supported chips can be found 
[here](https://brtchip.com/document/ic-datasheets).

Programming Guides can be found [here](https://brtchip.com/document/programming-guides/)

For exact download locations of each PDF file, see the Steve.h source file.

## Module Overview and Support Statement ##

* (EVE1)
  * FT800: 480x320,  18 bit RGB, 256K RAM, resistive  touch (NOT SUPPORTED)
  * FT801: 480x320,  18 bit RGB, 256K RAM, capacitive touch (NOT SUPPORTED)
* (EVE2)
  * FT810: 800x600,  18 bit RGB, 1MB RAM,  resistive  touch
  * FT811: 800x600,  18 bit RGB, 1MB RAM,  capacitive touch (TESTED)
  * FT812: 800x600,  24 bit RGB, 1MB RAM,  resistive  touch
  * FT813: 800x600,  24 bit RGB, 1MB RAM,  capacitive touch
* (EVE3)
  * BT815: 800x600,  24 bit RGB, 1MB RAM,  capacitive touch
  * BT816: 800x600,  24 bit RGB, 1MB RAM,  resistive  touch
* (EVE4)
  * BT817: 1280x800, 24 bit RGB, 1MB RAM,  capacitive touch (TESTED)
  * BT818: 1280x800, 24 bit RGB, 1MB RAM,  resistive  touch

NOTE: The FT800 and FT801 (EVE1, 480x320, 18 bit RGB, 256K RAM) work mostly the same as the supported modules; however, the memory map of the FT80X is different. Because of this, the FT800 and FT801 aren't supported for now.

