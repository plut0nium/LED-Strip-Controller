# LED strip controller
> A simple neopixel-like RGB LED strip control

## Features

 * HSV setting using 3 potentiometers
 * Manual mode and multiple pre-sets (up to 3 currently)
 * Uses the FastLED arduino library
 * Built with PlatformIO

## Usage
The three setting potentiometers should be wired between VCC and GND, and the wipers connected to A0..A2 respectively.  
Up to 4 NO momentary switches must be connected between pins 2..5 and GND, and RGB data out to pin 6.  

The first switch is used to enable manual mode, where LED color is adjusted using the three potentiometers.  
By pushing one of the three remaining switches >1s, the current setting will be saved to EEPROM (LEDs will blink black). Subsequently, a short push on the corresponding switch, will automatically load this saved setting from EEPROM.

## License

WTFPL - Do What The Fuck Tou Want To Public License
with the exception of the included `Button` library (BSD licensed)

