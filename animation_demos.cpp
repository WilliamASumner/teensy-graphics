#include "animation_demos.h" // Demo class
#include "tgraphics.h" // Timers/Pixel

//##################
// Simple Flash Demo 
//##################
// just choose a color and it will alternately flash the given color!

SimpleFlash::SimpleFlash(Pixel flash, uint32_t delayUs, float bright) {
  flashColor = flash;
  prevColor = Colors::Black;
  flip = false;
  palette = (Pixel*)&colorsExceptBlack;
  paletteLength = 12;
  delayInUs = delayUs;
  brightness = bright;
}

void SimpleFlash::setup(Pixel* pix, uint32_t radius, uint32_t diameter) {
  // Init array
  r = radius;
  d = diameter;
  pixels = pix;
  for (uint32_t i = 0; i < d; i++) {
    for (uint32_t j = 0; j < r; j++) { // along the radius same color
      pixels[indexAt(r,i,j)] = Colors::Black;
    }
  }
  timer.start(micros(),delayInUs);
}

void SimpleFlash::tick() {
  if (!timer.check()) { // timer still going
    return;
  } else {
    flip = !flip;
    timer.reset();
  }
  for (uint32_t i = 0; i < d; i++) {
    for (uint32_t j = 0; j < r; j++) {
     pixels[indexAt(r,i,j)] = flip ? flashColor * brightness : prevColor * brightness;
    }
  }
}

void SimpleFlash::processKeypress(uint16_t keys, uint16_t diff) {
  for (int i = 0; i < 12; i++) {
    if ((diff & 1) && (keys & 1)) {
      prevColor = flashColor;
      flashColor = palette[rand() % paletteLength];
      break;
    }
    diff >>= 1;
    keys >>= 1;
  }
}


//############################################################
// Rainbow wheel Demo, display a circular rainbow that rotates
//############################################################

RainbowWheel::RainbowWheel(float bright) {
  rainbowOffset = 0;
  brightness = bright;
}

void RainbowWheel::setup(Pixel* pix, uint32_t radius, uint32_t diameter) {
  // Init array
  r = radius;
  d = diameter;
  pixels = pix;
  for (uint32_t i = 0; i < d; i++) { // col #
    float percent = (float)((i+rainbowOffset) % d) / d; // fraction of the sweep
    Pixel col = rainbowAt(percent,rainbow12,12) * brightness;
    for (uint32_t j = 0; j < r; j++) { // ring # 
      pixels[indexAt(r,i,j)] = col;
    }
  }
  //timer.start(micros(),frameTime); // 10ms
}
void RainbowWheel::tick() {
    return; // do nothing
    /*
    for (uint32_t i = colOffset; i != (d + colOffset) % d; i = (i + 1) % d) { // across ring
        float percent = (float)((i+rainbowOffset) % d) / d; // fraction of the sweep
        Pixel col = rainbowAt(percent,rainbow12,12);
        for (uint32_t j = 0; j < r; j++) { // across radius
            pixels[indexAt(r,i,j)] =  col;
        }
    }
    */
}

void RainbowWheel::processKeypress(uint16_t keys, uint16_t diff) {
    
}

RingDemo::RingDemo(float bright, uint32_t fTime) {
  brightness = bright;
  frameTime = fTime;
}

void RingDemo::setup(Pixel* pix, uint32_t radius, uint32_t diameter) {
  // Init array
  r = radius;
  d = diameter;
  pixels = pix;
  for (uint32_t i = 0; i < d; i++) { // col #
      for (uint32_t j = 0; j < r; j++) { // ring # 
          pixels[indexAt(r,i,j)] = Colors::Black;
      }
  }
  timer.start(micros(),frameTime); // 10ms
}
void RingDemo::tick() {
}

void RingDemo::processKeypress(uint16_t keys, uint16_t diff) {
    
}
