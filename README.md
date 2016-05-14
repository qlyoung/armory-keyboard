armory-keyboard
===============

Utility for emulating a USB HID keyboard with the USBArmory.
Compatible with DuckyScript.

Building
--------
$ git clone https://github.com/qlyoung/armory-keyboard.git
$ cd armory-keyboard
$ make

You must build on the USBArmory or cross-compile.

Usage
-----
First, setup your USBArmory to emulate a USB HID keyboard, either manually through
ConfigFS or with the provided shell script ```hidnet.sh```.
```
$ ./type <script> [/dev/hidgX]
```

This utility understands plaintext (not compiled) [DuckyScripts](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript).
It will interpret the script and send the generated HID reports to the specified
HID character device, /dev/hidgX. If none is specified it will send to /dev/hidg0
by default.
