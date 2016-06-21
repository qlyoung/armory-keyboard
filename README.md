armory-keyboard
===============

Utility for emulating a USB HID keyboard with the USBArmory.  Kind of
compatible with DuckyScript.

[![asciicast](https://asciinema.org/a/45712.png)](https://asciinema.org/a/45712)

Building
--------
```shell
$ git clone https://github.com/qlyoung/armory-keyboard.git
$ cd armory-keyboard
$ make
```

The resultant binary is located in `build/`.  You must build on the USBArmory
or cross-compile.

Usage
-----
First, setup your USBArmory to emulate a USB HID keyboard, either
manually through ConfigFS or by executing the provided shell script `hid-ecm.sh`
(written by Collin Mulliner). The script expects to be running on the Debian
Jessie base image provided by Inverse Path, but should be easily adaptable to
other images.

Setting up HID Keyboard + CDC Ethernet:

```
# ./hid-ecm.sh
```

Using the utility:

```
# ./type -s <script file> -l <layout file> [-o <output file>]
```

The interpreter will interpret the script and send the generated HID reports to
the specified file. In the typical use case, this will be a HID character
device such as `/dev/hidg0`. If no device is specified, the default is
`/dev/hidg0`.

Scripts
-------
Originally, the interpreter was going to be compatible with
[DuckyScript](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript).
However, DuckyScript has weird and unnecessary limitations that don't really
correspond to the capabilities of HID, so instead I opted with a syntax based
mostly on DuckyScript with a few modifications to enhance capability and
ease-of-use. It should be fairly straightforward to convert DuckyScripts
to...uh...ArmoryDuckyScripts? We'll go with that.

Syntax is identical to DuckyScript, with the following exceptions:

* `CTRL-ALT`+key, `CTRL-SHIFT`+key, `ALT`+key, `SHIFT`+key, `CTRL`+key, and
  `GUI`+key combinations have all been replaced by a single keyword, `SIMUL`.
  Start a line with `SIMUL` and follow it with up to 6 space-delimited tokens or
  plaintext characters and all 6 will be sent in one HID report, as if they were
  pressed at the same time. Take `CTRL+ALT+DEL` as an example:

  DuckyScript: `CTRL-ALT DELETE`

  ArmoryDuckyScript: `SIMUL CTRL ALT DELETE`

  This seems more verbose at first, but it's actually better, because you can
  do things with it that you can't really do in DuckyScript, such as:

  `SIMUL CTRL ALT ENTER SPACE MENU a `

  I have no idea why you would want to do this, but HID supports sending up to
  6 keys per report, so I pass that option along to the user. Obviously it is up
  to you to send sane combinations, and up to the operating system to interpret
  them.  Note: all escape tokens (`SHIFT`, `CONTROL`, `SPACE`, etc) must occur
  before any plaintext characters.

* I haven't finished implementing all the syntax yet. Currently unimplemented
  are:

  * `REPEAT`, to repeat commands
  
  * `COMMAND` for OSX

* Lines may be at most 500 characters. Excess characters will be ignored.

* `DEFAULT_DELAY` may occur at any point in the script, and overrides the
  previous default delay.

Examples are located in the `examples/` directory.

See the
[DuckyScript](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript)
wiki for further documentation on syntax.

Layouts
-------
Support for Unicode and arbitrary keyboard layouts was added in [c0e032c](https://github.com/qlyoung/armory-keyboard/commit/c0e032cd852b6b31bd2638d798e727915d035cdc). Layout files may be specified with the `-l` option. The format of a layout file is as follows:

```
-*- layout: <layout name> -*-

<unicode char> 0x<keycode> 0x<modifier bitfield>
<unicode char> 0x<keycode> 0x<modifier bitfield>
<unicode char> 0x<keycode> 0x<modifier bitfield>
...
```

The first line must contain the string `-*- layout: ` followed by a name for the layout. The terminating `-*-` is not strictly necessary.
Subsequent lines must contain the following, in order:

 1. A UTF-8 character
 2. Unicode/ASCII space (`0x20`)
 3. The string `0x` followed by the hexadecimal keycode for the key that should be pressed to produce (1)
 4. Unicode/ASCII space (`0x20`)
 5. The string `0x` followed by the hexadecimal bitfield specifying which modifier keys must be pressed in tandem with (3) to produce (1)

Here is a brief excerpt of the English/US layout:
```
-*- layout: English 103P (USA) -*-

a 0x04 0x00
b 0x05 0x00
c 0x06 0x00
d 0x07 0x00
e 0x08 0x00
f 0x09 0x00
```

And of the Albanian 452 layout:

```
-*- layout: Albanian 452 -*-

` 0x35 0x00
¬ 0x35 0x02
| 0x35 0x40
1 0x1E 0x00
! 0x1E 0x02
2 0x1F 0x00
```

For your convenience, I compiled a map of keycodes for a standard keyboard. Characters for the English/US layout have been left in for readability, but regardless of what character is on a given key the keycode is the same. With this map and a little patience it is easy to add support for whatever keyboard layout you wish.

![keycode mappings for standard keyboard](https://raw.githubusercontent.com/qlyoung/armory-keyboard/master/layouts/keyboard-layout.png)



Contributing
------------
Fork & PR! Open to improvements and bugfixes (there
are almost certainly bugs).

Wishlist / Planned Features:
* HID support for [libusbgx](https://github.com/libusbgx/libusbgx)
* ~~International layout support~~

License
-------
```
Copyright 2016  Quentin Young

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/.
```
