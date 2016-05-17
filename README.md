armory-keyboard
===============

Utility for emulating a USB HID keyboard with the USBArmory.
Kind of compatible with DuckyScript.

[![asciicast](https://asciinema.org/a/45712.png)](https://asciinema.org/a/45712)

Building
--------
```shell
$ git clone https://github.com/qlyoung/armory-keyboard.git
$ cd armory-keyboard
$ make
```

You must build on the USBArmory or cross-compile.

Usage
-----
First, setup your USBArmory to emulate a USB HID keyboard, either manually through
ConfigFS or by exeuting the provided shell script ```hidnet.sh``` (written by Collin
Mulliner). The script expects to be running on the Debian Jessie base image provided
by Inverse Path, but should be easily adaptable to other images.

Setting up HID Keyboard + CDC Ethernet:
```
# ./hidnet.sh
```
Using the utility:
```
# ./type <script> [/dev/hidgX]
```

The interpreter will interpret the script and send the generated HID reports to the
specified HID character device, `/dev/hidgX`. If no device is speficied, the default is
`/dev/hidg0`.

Scripts
-------
Originally, the interpreter was going to be compatible with [DuckyScript](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript).
However, DuckyScript has weird and unnecessary limitations that don't really correspond
to the capabilities of HID, so instead I opted with a syntax based mostly on DuckyScript
with a few modifications to enhance capability and ease-of-use. It should be fairly
straightforward to convert DuckyScripts to...uh...ArmoryDuckyScripts? We'll go with that.

Syntax is identical to DuckyScript, with the following exceptions:

* `CTRL-ALT`, `CTRL-SHIFT`, and other such combined keys have been replaced by a single keyword,
  `SIMUL`. Start a line with `SIMUL` and follow it with up to 6 tokens or plaintext characters
  and all 6 will be sent in one HID report, as if they were pressed at the same time. Take `CTRL+ALT+DEL`
  as an example:

  DuckyScript: `CTRL-ALT DELETE`

  ArmoryDuckyScript: `SIMUL CTRL ALT DELETE`

  This seems more verbose at first, but it's actually better, because you can do things with
  it that you can't really do in DuckyScript, such as:

  `SIMUL CTRL ALT ENTER SPACE MENU a `

  I have no idea why you would want to do this, but HID supports sending up to 6 keys per
  report, so I pass that option along to the user. Obviously it is up to you to send sane
  combinations, and up to the operating system to interpret them.
  Note: all escape tokens (`SHIFT`, `CONTROL`, `SPACE`, etc) must occur before any plaintext characters.

* I haven't finished implementing all the syntax yet. Currently unimplemented are:
 * `REPEAT`, to repeat commands
 * `ALT-SHIFT`, to switch keyboard layouts (not yet supported)

* Lines may be at most 500 characters. Excess characters will be ignored.

* DEFAULT_DELAY may occur at any point in the script, and overrides the previous default delay.

Examples are located in the `examples/` directory.

See the [DuckyScript](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript) wiki for further documentation on syntax.

Contributing
------------
Fork & PR! Open to improvements and bugfixes (there are almost certainly bugs).

Wishlist / Planned Features:
* HID support for [libusbgx](https://github.com/libusbgx/libusbgx)
* International layout support

License
-------
```
Copyright 2016  Quentin Young

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
```
