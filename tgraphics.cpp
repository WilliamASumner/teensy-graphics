#include "tgraphics.h"


// Note: not all RGB combinations are specified, just RGB < - > BGR (what the TLCS are set up to do)
// To add one, just follow the examples below for rgb_struct and modify animation_lib.{h,cpp} accordingly

FrameTimer::FrameTimer() {
  frameTarget = 0;
  frameCounter = 0;
}

FrameTimer::FrameTimer(uint32_t target) {
  frameTarget = target;
  frameCounter = 0;
}

bool FrameTimer::check() { // returns true if expired
  if (frameCounter >= frameTarget)
    return true;
  frameCounter++;
  return false;
}

void FrameTimer::reset() {
  frameCounter = 0;
}

void FrameTimer::update(uint32_t target) {
  frameTarget = target;
  frameCounter = 0; // Note: resets counter
}

SimpleTimer::SimpleTimer() {
  microStart = 0;
  microDuration = 0;
}


SimpleTimer::SimpleTimer(uint32_t uStart, uint32_t uDuration) {
  microStart = uStart;
  microDuration = uDuration;
}

void SimpleTimer::start(uint32_t uStart, uint32_t uDuration) {
  microStart = uStart;
  microDuration = uDuration;
}


// Note: if you wait too long this will overflow in ~7.6s
bool SimpleTimer::check() {
  uint64_t temp = micros() - microStart;
  if (temp >= microDuration)
    return true;
  return false;
}

void SimpleTimer::reset() {
  microStart = micros();
}

void SimpleTimer::update(uint32_t target) {
  microDuration = target;
  microStart = micros(); // Note: resets counter
}
