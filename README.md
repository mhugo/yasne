# Yet Another Sh----y NES Emulator

YASNES is an unfinished NES emulator written in C++.

It is the result of a personal experiment about how to write an emulator. The NES platform has been chosen because of the fond memories of my childhood I have with it and the [comprehensive documentation one can found](https://www.nesdev.org/wiki/Nesdev_Wiki).

Overall it was a very joyful experience. Being able to play Super Mario Bros, even at a very low framerate, without sound and with visual glitches is fascinating. It is a bit as if I made a cool game, except I did not of course. Watching the game being playable little by little for each small bugfix added gives a very nice motivation.

The current state is something like:
- the CPU emulation should be close to 100% ok,
- the PPU (graphics unit) has bugs that result in strange colors and some glitches around sprites (see Super Mario Bros scren shots below),
- the APU (sound unit) is not covered at all

No optimisation has been investigated, the emulation is quite naive and slow, even on modern pieces of hardware.

## Compilation

You'll need SDL 2 to compile it (`sudo apt install libsdl2-dev` on Debian-like systems)

```
mkdir build
cd build
cmake ..
make
```

A [CPU test suite](data/nestest.nes) is provided. You can run it with e.g.: `./nes ../data/nestest.nes`

## Embedded debugger

The emulator starts paused on the first instruction and a small debugger prompt.

The following commands are available:

- `c` : continue the emulation
- `s` (step) : execute the next instruction and pause again
- `b <addr>` : break when the address `addr` is encountered
- `bf` : break on next video frame
- `bdf` : disable break on video frame
- `bd` : disable breakpoints
- `v` : dump some PPU internals
- `vdump <file>` : dump the current video memory to a file
- `p <sprite>` : output the content of the sprite number `<sprite>` as text
- `k` : display current controller state
- `k a|b|select|start|up|down|right|left 0|1` : set controller button's state
- `w <addr> r|w` : add a watch on a memory address
- `q` : quit

## Keyboard controls

There is only one game controller hard mapped to the host keyboard with:
- `a` -> A button
- `b` -> B button
- `<space>` -> select
- `<return>` -> start
- `<left>` -> left
- `<right>` -> right
- `<up>` -> up
- `<down>` -> down

The `p` key allows to pause the emulation and switch to the embedded debugger.

## Screenshots

![NEST test](https://github.com/mhugo/yasne/blob/master/screenshots/nestest.png?raw=true)

![SMB](https://github.com/mhugo/yasne/blob/master/screenshots/smb.png?raw=true)

