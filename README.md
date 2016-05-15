armory-keyboard
===============

Utility for emulating a USB HID keyboard with the USBArmory.
Kind of compatible with DuckyScript.

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
ConfigFS or with the provided shell script ```hidnet.sh```.

```
$ ./type <script> [/dev/hidgX]
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
  and all 6 will be sent in one HID report, as if they were pressed at the same time.

Example:

DuckyScript: `CTRL-ALT DELETE`

ArmoryDuckyScript: `SIMUL CTRL ALT DELETE`

This seems more verbose at first, but it's actually better, because you can do things with
it that you can't really do in DuckyScript, such as:

`SIMUL CTRL ALT ENTER a SPACE MENU`

I have no idea why you would want to do this, but HID supports sending up to 6 keys per
report, so I pass that option along to the user. Obviously it is up to you to send sane
combinations, and up to the operating system to interpret them.

* I haven't finished implementing all the keycodes yet. Currently unimplemented are:
 * `REP`, to repeat commands
 * `SCROLLOCK`
 * `PRINTSCREEN`
 * `BREAK/PAUSE`
 * `F1` - `F12`

Beyond that, everything else works as expected:

```
DEFAULT_DELAY 1000
SIMUL GUI d
SIMUL GUI r
STRING notepad.exe
ENTER
STRING ayyy lmao
HOME
ENTER
UPARROW
STRING ddddd
DELETE
SIMUL CTRL S
STRING memes.txt
ENTER
```
etc.

See the DuckyScript wiki for further info.

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
