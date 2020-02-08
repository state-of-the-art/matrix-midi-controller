# SotA Matrix MIDI Controller

Simple DIY prototype of MIDI controller based on Teensy & Adafruit NeoTrellis board inspired by
awesome but expensive [Monome Grid](https://monome.org/docs/grid/) and
[Linnstrument](http://www.rogerlinndesign.com/linnstrument.html).

* [Site page about it](https://www.state-of-the-art.io/projects/matrix-midi-controller/)

## Features

* USB MIDI output: multiple simultaneous active buttons
* USB MIDI input: highlight keys, clock indication
* Flexible keys & menu layout configuration (by change src right now)
* Adjustable brightness & options to disable the helper indicators
* Adjustable velocity and tuning
* Menu buttons hints by long press on interesting button

## Plans

If the project will get some good feedback - we can think about kickstarter with fixed issues and maybe
with additional features like linnstrument have - but for much less price (~$200-300). So don't hesitate
and say what do you think about it :)

## Components

* 12x$11.25 Adafruit NeoTrellis RGB Driver PCB for 4x4 Keypad - [Adafruit](https://www.adafruit.com/product/3954)
[Amazon](https://www.amazon.com/dp/B07L5Y9M2P/)
* 12x$4.46 Silicone Elastomer 4x4 Button Keypad - for 3mm LEDs - [Adafruit](https://www.adafruit.com/product/1611)
[Amazon](https://www.amazon.com/dp/B00SK8O5D2/)
* $19.80 Teensy 3.2 - [PJRC](https://www.pjrc.com/store/teensy32.html)
[Amazon](https://www.amazon.com/dp/B015M3K5NG/)
* Number of wires, micro-usb cable, small screws, 3d printer to print the enclosure (top cover is available)

Total: ~$210

### Options

* You can try to use another control board, that supports i2c & usb MIDI.
* Also you can try to use another number of boards to get some different configuration

If you change something, most probably you will need to adjust the source code.

## Known issues

* Adafruit NeoTrellis PCB:
    * Button can touch led terminals and activate the vertical line of board buttons - you can insulate the
    terminals using glue or any other way to fix this.
    * Buttons is quite hard to operate - easy to hit, but to press a number of buttons you need to practice

You also can check the issues page on GitHub:
* [Issues](https://github.com/state-of-the-art/matrix-midi-controller/issues)

## Arduino IDE & build

It's a good idea to find the teensy board page and follow the instructions:

* [Teensyduino](https://www.pjrc.com/teensy/td_download.html)

To build - just open the sketch with Arduino IDE and click Verify button.
To flash - connect the Teensy board, select the port & click Upload button.

### Configs

* Board: Teensy 3.2
* USB Type: Serial + MIDI
* CPU Speed: 72MHz

## OpenSource

This is an experimental project - main goal is to test State Of The Art philosophy on practice.

We would like to see a number of independent developers working on the same project issues
for the real money (attached to the ticket) or just for fun. So let's see how this will work.

### License

Repository and it's content is covered by `Apache v2.0` - so anyone can use it without any concerns.

If you will have some time - it will be great to see your changes merged to the original repository -
but it's your choise, no pressure.
