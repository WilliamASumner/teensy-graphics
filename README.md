# A simple library for doing graphics with Teensy

## Setup
Simply `git clone` this repository into your Arduino libraries folder (something like ~/Arduino/libraries) and use `#include <tgraphics.h>` for lower-level graphics functions or just `#include <animation_demos.h>` if you want to mess with the pre-made animations.

## Demo Class
If you want to write an animation for the [wallytron](https://github.com/WilliamASumner/wallytron) project, this class provides an easy to use interface. All you need is a `void setup(Pixel* displayBuffer, uint16_t rowSize, uint16_t ringSize)` a `void processKeys(uint16_t, uint16_t)` and a `void tick(...)` function. Setup is run once at the beginning of the animation and could do something like initialize the displayBuffer to all `Colors::Black`, for example. Next, the processKeys function will react to a key press. This could modify your animation in a number of ways, like causing the color to change or clearing the display. Just keep in mind that a keypress is generated for both press and un-press events so be sure to use the diff value to figure out what happened if that matters. Finally, the `tick` function is the main workhorse, it's called whenever the µController has some free time to update the animation. For that reason you might want to define a `tick(uint16_t colNumber)` and check where the display is so you don't update the displayBuffer in a weird way. Note: This problem may be mitigated if I decide to implement a double-buffering system for wallytron.

## Graphics Functions and Values
### Colors
`RGB_Color`s are the basic building block of the library, underneath they are simply `uint16_t` variables for easy use with the [TLC5948](https://github.com/WilliamASumner/Tlc5948). In `tgraphics.h` there are a number of predefined colors such as `Colors::Red`, `Colors::DodgerBlue` and `Colors::Chartreuse`. There are also several palettes such as the default `colorPalette` and `colorsExceptBlack` that may be easier to use with some functions.

### Pixels
`Pixel`s are built on top of `Colors`, they add some basic functions like `+` and `*` operations to make it easier to work with. An example usage could be:
```C
Pixel red = Colors::Red;
Pixel blue = Colors::Blue;
Pixel purple = (red * 0.5) + (blue * 0.5)
```

### Other Functions
There are also `lerp`ing and `vec` functions, among others, that can be useful:
`vecFade` - fades an array of a certain size by subtracting a `uint16_t` amount from each element
`vecFill` - fills an array with a specified `Pixel*`, `Pixel` or `uint16_t`
`lerp_float`/`lerp_uint` - interpolates between two `Pixel`s with a `float` or `uint16_t`
`SimpleTimer` - this class lets you create a simple counter for the number of microseconds that have elapsed, and `.check()`ing it will tell you if it has gone off or not. It's only good for ~7.6s before it overflows, though.
`vecBlur` - blurs between `Pixel`s along an array, smearing everything together and also losing a bit of brightness (i.e. eventually an array will fade to black if repeatedly blurred).
`vecBrighten` - as the name states, it brightens an array by a `uint16_t`
`rainbowAt` - Takes a fraction from 0-1, a rainbow palette/table and a palette/table size. The result is a color at that point in the rainbow, so if you call this function with values from 0.0 - 1.0, it will create a smooth transition between all the colors in the palette.
