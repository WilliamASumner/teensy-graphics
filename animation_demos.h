#ifndef __ANIMATION_DEMOS_H
#define __ANIMATION_DEMOS_H
#include "tgraphics.h"
#include <cstdint>

// How a demo works
// void demo_XXX.setup(...); // sets up initial pixel state (best to call in setup())

// void demo_XXX.updateArray(...); // generates next full-frame of pixel data

class Demo {
  public:
    virtual void setup(Pixel* pixels, uint32_t radius, uint32_t diameter);
    virtual void tick();
    virtual void processKeypress(uint16_t keys, uint16_t diff);
  protected:
    uint32_t r;
    uint32_t d;
    Pixel* pixels;
};

// Flashing Demo
class SimpleFlash : public Demo {
  public:
    SimpleFlash(Pixel flash, uint32_t delayUs, float brightness);
    void setup(Pixel* pixels, uint32_t radius, uint32_t diameter);
    void tick();
    void processKeypress(uint16_t keys, uint16_t diff);
  private:
    SimpleTimer timer;
    Pixel* palette;
    Pixel flashColor;
    Pixel prevColor;
    uint16_t paletteLength;
    uint32_t delayInUs;
    bool flip;
    float brightness;
};


// Rainbow Demo
class RainbowWheel : public Demo {
  public:
    RainbowWheel(float brightness);
    void setup(Pixel* pixels, uint32_t w, uint32_t h);
    void tick();
    void processKeypress(uint16_t keys, uint16_t diff);
  private:
    SimpleTimer timer;
    int rainbowOffset;
    uint32_t frameTime;
    float brightness;
};

// Oscillating ring
class RingDemo : public Demo {
  public:
    RingDemo(float brightness, uint32_t frameTime);
    void setup(Pixel* pixels, uint32_t w, uint32_t h);
    void tick();
    void processKeypress(uint16_t keys, uint16_t diff);
  private:
    SimpleTimer timer;
    uint32_t frameTime;
    float brightness;
};

#endif // ifndef __ANIMATION_DEMOS_H
