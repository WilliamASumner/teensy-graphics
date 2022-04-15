#ifndef __ANIMATION_LIB_H
#define __ANIMATION_LIB_H
#include <Arduino.h> // micros
#include <arm_math.h>
#include <cstdint>

// Animation ideas
// Moving dots (flying around bouncing, added with each keypress)
// Fireworks (shot after each key press)
// Fade to hue by note
// Rotating spiral
// Rings (lit up by note)

// Data types
// mainly going to use uint16_t, treating as unsigned to get maximum fidelity out of PWM drivers (16 bit)
// for saturating arm funcs, we'll use Q15 with a recast
// Note: This does cause an issue with saturating math...
// ... because the Q15 format uses a sign bit, any additions above 7F with positive numbers ( < 7FFF)
// will saturate to just 7F. So, as long as we avoid > 7FFF additions/insertions we can get away with
// it but will only have 15 bits resolution
// to make it as invisible as possible, it's important to call a shift function (vecShiftFast)
// to shift the missing bit to the LSB, otherwise the LEDs won't get close to full brightness


// What functions do I need
// Vector fill
// Vector add (automatically saturating)
// Vector sub?
// fade (mix with black)
// blur (mix with colors)
// sin

// Data structures
// Triple ring buffer - one for display, one for computation, one for final shift
// TODO: see if we can't max this take less space ^^^
// ring buffer -> modulo must be done from last col index to first col index.

// If the performance is not good enough, then maybe try <arm_math.h> CMSIS DSP instructions

/*-- Example diagram of POV display:

                    Rows are these 'rings'
                           ______
                          / ____ \
                         / / __ \ \
                        / / /  \ \ \
                        \ \ \__/ / /
                         \ \____/ /
                          \______/

           Columns are these lines sweeping across spin

                          \  |  /
                           \ | /
                            \|/
                      -------|-------
                            /|\
                           / | \
                          /  |  \

    For correct blurring/etc we need blurring to blend first/last row together

    The XY display is made by 'unwrapping the rings' and creating a rectangle from the circular display

*/


// Basic saturating math functions
// stolen from https://stackoverflow.com/questions/121240/how-to-do-unsigned-saturating-addition-in-c
inline uint16_t qadd16(uint16_t a, uint16_t b) {
    uint16_t c = a + b;
    if (c < a)
        c = 0xFFFF;
    return c;
}

inline uint16_t qsub16(uint16_t a, uint16_t b) {
    uint16_t c = a - b;
    if (c > a)
        c = 0;
    return c;
}

inline uint16_t qmult16(uint16_t a, uint16_t b) {
    uint16_t c = a * b;
    if (c < a)
        c = 0xFFFF;
    return c;
}

inline uint16_t qmult16(uint16_t a, float b) {
    uint16_t c = a * b;
    if (b < 0.0)
        c = 0x0;
    if (c < a && b > 1.0)
        c = 0xFFFF;
    return c;
}

// division is already saturating for uints

struct rgb_struct;
struct rbg_struct;
struct bgr_struct;
struct brg_struct;
struct gbr_struct;
struct grb_struct;

struct Pixel {
    uint16_t blue;
    uint16_t green;
    uint16_t red;

    friend Pixel operator+(Pixel lhs, const Pixel& rhs) {
        lhs.blue = qadd16(lhs.blue,rhs.blue);
        lhs.green = qadd16(lhs.green,rhs.green);
        lhs.red = qadd16(lhs.red,rhs.red);
        return lhs;
    }

    friend Pixel operator+(Pixel lhs, uint16_t rhs) {
        lhs.blue = qadd16(lhs.blue,rhs);
        lhs.green = qadd16(lhs.green,rhs);
        lhs.red = qadd16(lhs.red,rhs);
        return lhs;
    }

    friend Pixel operator-(Pixel lhs, const Pixel& rhs) {
        lhs.blue = qsub16(lhs.blue,rhs.blue);
        lhs.green = qsub16(lhs.green,rhs.green);
        lhs.red = qsub16(lhs.red,rhs.red);
        return lhs;
    }

    friend Pixel operator-(Pixel lhs, uint16_t rhs) {
        lhs.blue = qsub16(lhs.blue,rhs);
        lhs.green = qsub16(lhs.green,rhs);
        lhs.red = qsub16(lhs.red,rhs);
        return lhs;
    }


    friend Pixel operator*(Pixel lhs, const Pixel& rhs) {
        lhs.blue = qmult16(lhs.blue,rhs.blue);
        lhs.green = qmult16(lhs.green,rhs.green);
        lhs.red = qmult16(lhs.red,rhs.red);
        return lhs;
    }

    friend Pixel operator*(Pixel lhs, float rhs) {
        lhs.blue = qmult16(lhs.blue,rhs);
        lhs.green = qmult16(lhs.green,rhs);
        lhs.red = qmult16(lhs.red,rhs);
        return lhs;
    }
    bool operator==(const Pixel &other) {
        return red == other.red && blue == other.blue && green == other.green;
    }
};


inline Pixel lerp_float(const Pixel& a, const Pixel& b, float frac) {
    if (frac > 1.0) frac = 1.0;
    else if (frac < 0.0) frac = 0.0;
    return a * frac + b * (1. - frac);
}

inline Pixel lerp_uint(const Pixel& a, const Pixel& b, uint16_t frac) {
    return lerp_float(a,b,((float)frac / 65535.));
}



struct rgb_struct {
  uint16_t r, g , b;
  operator Pixel() const { return {.blue = b, .green = g, .red = r}; }
};

struct rbg_struct {
  uint16_t r, b, g;
  operator Pixel() const { return {.blue = b, .green = g, .red = r}; }
};

struct bgr_struct {
  uint16_t b, g, r;
  operator Pixel() const { return {.blue = b, .green = g, .red = r}; }
};

struct brg_struct {
  uint16_t b, r, g;
  operator Pixel() const { return {.blue = b, .green = g, .red = r}; }
};

struct gbr_struct {
  uint16_t g, b, r;
  operator Pixel() const { return {.blue = b, .green = g, .red = r}; }
} ;

struct grb_struct {
  uint16_t g, r, b;
  operator Pixel() const { return {.blue = b, .green = g, .red = r}; }
};

using RGB_Color = rgb_struct;
using RBG_Color = rbg_struct;
using BGR_Color = bgr_struct;
using BRG_Color = brg_struct;
using GBR_Color = gbr_struct;
using GRB_Color = grb_struct;


class FrameTimer {
  public:
    uint32_t frameCounter;
    uint32_t frameTarget;
    FrameTimer();
    FrameTimer(uint32_t target);
    bool check();
    void reset();
    void update(uint32_t target);
};

class SimpleTimer {
  public:
    uint32_t microStart, microDuration;
    SimpleTimer();
    SimpleTimer(uint32_t uStart, uint32_t uDuration);
    void start(uint32_t uStart, uint32_t uDuration);

    // Note: if you wait too long this will overflow in ~7.6s
    bool check();
    void reset();
    void update(uint32_t target);
};


inline uint32_t indexAt(uint32_t colSize, uint32_t col, uint32_t ring) {
  return colSize * col + ring;
}


enum class EdgeType : uint16_t {
  Wrap = 1,
  Reflect = 2,
  Copy = 3,
  Black = 4,
};

inline void vecTransposeFast(uint16_t* src,
                             uint16_t srcRows,
                             uint16_t srcCols,
                             uint16_t* dst,
                             uint16_t dstRows,
                             uint16_t dstCols) {
  arm_matrix_instance_q15 srcMat = { srcRows, srcCols, (q15_t*)src };
  arm_matrix_instance_q15 dstMat = { dstRows, dstCols, (q15_t*)dst };
  arm_mat_trans_q15(&srcMat, &dstMat);

}

inline uint16_t getEdgeColor(
  EdgeType e,
  uint16_t* src,
  int32_t indX,
  int32_t indY,
  int32_t px,
  int32_t py,
  int32_t xSize,
  int32_t ySize,
  int32_t color) {
  uint32_t xCoor = 0, yCoor = 0;
  switch (e) {
    case EdgeType::Wrap:// use overflow
      if (indX < 0 || indX >= xSize) {
        xCoor = xSize + (uint32_t)indX;
      }
      if (indY < 0 || indY >= ySize) {
        yCoor = ySize + (uint32_t)indY;
      }
    case EdgeType::Reflect:
      if (indX < 0) {
        xCoor = -indX - 1;
      } else if (indX >= xSize) {
        xCoor = 2 * xSize - indX - 1;
      }
      if (indY < 0) {
        yCoor = -indY - 1;
      } else if (indY >= ySize) {
        yCoor = 2 * ySize - indY - 1;
      }
      break;
    case EdgeType::Copy:
      xCoor = px;
      yCoor = py;
      break;
    default: // default to black
    case EdgeType::Black:
      return 0xffff;
  }

  return src[(xCoor * xSize + yCoor) * 3 + color];
}

inline uint16_t saturatingAdd(uint16_t a, uint16_t b) {
  uint32_t temp = (uint32_t)a + (uint32_t)b;
  if (temp > 0xffff)
    temp = 0xffff;
  return temp;
}

// Convolve a 2D array with a 2D kernel (separated)
// Note: Don't give this function more than 2^31-1 sized arrays

inline void convolveSeparable(uint16_t* src,
                              uint32_t  srcSizeX,
                              uint32_t  srcSizeY,
                              float* vecKernelX,
                              uint32_t  kernelSizeX,
                              float* vecKernelY,
                              uint32_t  kernelSizeY,
                              EdgeType  edgeHandling,
                              uint16_t* dst) { // assumes dst is the same dimensions as src


  for (uint32_t py = 0; py < srcSizeY; py++) {
    for (uint32_t px = 0; px < srcSizeX; px++) {
      // for each pixel src[px][py]

      int32_t kPixelOffset = 0;
      int32_t baseIndex = (px + py * srcSizeX) * 3;
      // Apply X-Kernel
      for (uint32_t kx = 0; kx < kernelSizeX; kx++) {
        kPixelOffset = kx - kernelSizeX / 2; // ranges from -kernelSizeX/2 <-> +kernelSizeX/2

        // if out of bounds for x
        if (px + kPixelOffset < 0 || px + kPixelOffset >= srcSizeX ) {
          for (int c = 0; c < 3; c++) { // colors
            dst[baseIndex + c] =
              saturatingAdd(getEdgeColor(edgeHandling,
                                         src,
                                         baseIndex + (kPixelOffset * 3) + c,
                                         py,
                                         px,
                                         py,
                                         srcSizeX,
                                         srcSizeY,
                                         c),
                            dst[baseIndex + c]);
          }
        } else {
          for (int c = 0; c < 3; c++) { // colors
            //printf("pixel (%d,%d)[%d]:%d, kernelX at %d, kernelOffset %d\n",px,py,baseIndex,c,kx,kPixelOffset);
            //printf("adding src[%d] (=%d) * %f (=%f) + dst[%d] (=%d)\n",baseIndex + kPixelOffset * 3 + c,src[baseIndex + kPixelOffset * 3 + c],vecKernelX[kx], src[baseIndex + kPixelOffset * 3 + c] * vecKernelX[kx],baseIndex+c,dst[baseIndex+c]);
            dst[baseIndex + c] =
              saturatingAdd(src[baseIndex + (kPixelOffset * 3) + c] * vecKernelX[kx], dst[baseIndex + c]); // currPixel * kernel
            //printf("new dst[] = %x\n\n",dst[baseIndex+c]);
          }
        }
      }

      // Since it's separable, no need to do nested kernel loop
      // Apply Y-Kernel
      for (uint32_t ky = 0; ky < kernelSizeY; ky++) {
        // ranges from -kernelSizeX/2 <-> +kernelSizeX/2
        kPixelOffset = (ky - kernelSizeY / 2);
        // if out of bounds for y
        if (py + kPixelOffset < 0 || py + kPixelOffset >= srcSizeY) {
          for (int c = 0; c < 3; c++) {
            dst[baseIndex + c] =
              saturatingAdd(getEdgeColor(edgeHandling,
                                         src,
                                         px,
                                         py,
                                         px,
                                         py,
                                         srcSizeX,
                                         srcSizeY,
                                         c),
                            dst[baseIndex + c]);
          }

        } else {
          kPixelOffset *= srcSizeX;
          for (int c = 0; c < 3; c++) {
            dst[baseIndex + c] =
              saturatingAdd(src[baseIndex + (kPixelOffset * 3) + c] * vecKernelY[ky], dst[baseIndex + c]);
          }
        }
      }
    }
  }
}



inline uint16_t* blur2d(uint16_t* myImg, int32_t imgWidth, int32_t imgHeight, uint16_t*myImgResult) {
  float myKernelX[] = {1 / 3.0, 1 / 3.0, 1 / 3.0};
  float myKernelY[] = {1 / 3.0, 1 / 3.0, 1 / 3.0};

  convolveSeparable(myImg, imgWidth, imgHeight,
                    myKernelX, 3, myKernelY, 3,
                    EdgeType::Black, myImgResult);

  return myImgResult;
}

// ~4295s overflow on micros() -> 0.000232 Hz
// micros overflows 
inline uint16_t beat16(float hz, uint32_t offset) {
    return ((micros() - offset)  * (uint16_t)(4295 * hz)) >> 16;
}

inline uint16_t beatSine16Unit(float hz, uint32_t offset) {
    float x = (float)beat16(hz,offset);
    x /= 65536.0;
    return (uint16_t)( 32767 * (20.785 * x * (x-0.5) * (x-1.0)) + 32768);
}

inline uint16_t beatSine16(float hz, uint32_t offset, uint16_t lo, uint16_t hi) {
    uint16_t baseSine = beatSine16Unit(hz,offset);
    float range = ((float)hi - (float)lo) / 65535.;
    baseSine = qmult16(baseSine,range);
    return qadd16(baseSine,lo);
}

//-------------------------//

// Default 'slow'/non-saturating version

//-------------------------//

inline void vecFill(uint16_t* src, uint16_t* dst, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = src[i];
  }
}

inline void vecFill(uint16_t* src, uint16_t* dst, uint32_t numElems, uint16_t mod) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = src[i % mod];
  }
}

inline void vecFill(const Pixel* src, Pixel* dst, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = src[i];
  }
}

inline void vecFill(const Pixel* src, Pixel* dst, uint32_t numElems, uint16_t mod) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = src[i % mod];
  }
}

inline void vecFill(const Pixel src, Pixel* dst, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = src;
  }
}

inline void vecAdd(uint16_t* src, uint16_t* dst, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] += src[i];
  }
}

inline void vecFade(uint16_t* src, uint16_t* dst, uint16_t fadeAmt, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = qsub16(src[i], fadeAmt);
  }
}

inline void vecFade(Pixel* src, Pixel* dst, uint16_t fadeAmt, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = src[i] - fadeAmt;
  }
}

inline void vecBrighten(uint16_t* src, uint16_t* dst, uint16_t fadeAmt, uint32_t numElems) {
  for (uint32_t i = 0; i < numElems; i++) {
    dst[i] = qadd16(src[i], fadeAmt);
  }
}

// Average with 4% loss on purpose
inline Pixel avg(const Pixel& one, const Pixel& two) {
    return one * 0.48f + two * 0.48f;
}


// blur amt go from totally even avg (1.0) to only center pixel color (0.0)
inline Pixel avg(const Pixel& l, const Pixel& mid, const Pixel& r, float blurAmt) {
    float sides = (blurAmt - 0.05) / 3.;
    float center = 1.0 - sides * 2;
    return l * sides + mid * center + r * sides;
}

inline void vecBlur(Pixel* src, Pixel* dst, float blurAmt, uint32_t numElems) {
    blurAmt = blurAmt > 1.0 ? 1.0 : blurAmt;
    blurAmt = blurAmt < 0.0 ? 0.0 : blurAmt;

    for (uint32_t i = 0; i < numElems; i++) {
        dst[i] = avg(src[(i+1) % numElems],
                     src[i],
                     src[(numElems + (i-1)) % numElems],
                     blurAmt);
    }
}

// interpolates across a table
inline Pixel rainbowAt(float percent, const Pixel* rainbowTable, uint32_t tableSize) {
    float adjPercent = tableSize * percent;
    double intPart = 0, fracPart = 0;
    fracPart = 1.0 - modf(adjPercent,&intPart);
    uint32_t index = (uint32_t)intPart;

  return lerp_float(rainbowTable[index],rainbowTable[(index + 1) % tableSize],fracPart);
}

inline void printPixel(const Pixel& p) {
    Serial.print("0x");
    Serial.print(p.red,HEX);
    Serial.print(", 0x");
    Serial.print(p.green,HEX);
    Serial.print(", 0x");
    Serial.print(p.blue,HEX);
    Serial.print(" ; ");
}

inline void vecPrint(Pixel* src, uint32_t numElems) {
    for (uint32_t i = 0; i < numElems; i++) {
        printPixel(src[i]);
    }
    Serial.println();
}

inline void printGsBuffer(uint16_t *buff, uint32_t numElems) {
    for (uint32_t i = 0; i < numElems; i++) {
        Serial.print("0x");
        Serial.print(buff[i],HEX);
        Serial.print("  ");
        if (i % 3 == 2) // divide on pixels
            Serial.print("| ");
        if (i % 8 == 7) // new lines to keep tidy
            Serial.println();
    }
}

namespace Colors {
    const RGB_Color Black =       { 0x0000, 0x0000, 0x0000 };
    const RGB_Color White =       { 0x00ff, 0x00ff, 0x00ff };
    // 12 color Rainbow!
    const RGB_Color Red =         { 0x00ff, 0x0000, 0x0000 };
    const RGB_Color Orange =      { 0x00ff, 0x007f, 0x0000 };
    const RGB_Color Yellow =      { 0x00ff, 0x00ff, 0x0000 };
    const RGB_Color Chartreuse =  { 0x007f, 0x00ff, 0x0000 };
    const RGB_Color Green =       { 0x0000, 0x00ff, 0x0000 };
    const RGB_Color SpringGreen = { 0x0000, 0x00ff, 0x007f };
    const RGB_Color Cyan =        { 0x0000, 0x00ff, 0x00ff };
    const RGB_Color DodgerBlue =  { 0x0000, 0x007f, 0x00ff };
    const RGB_Color Blue =        { 0x0000, 0x0000, 0x00ff };
    const RGB_Color Purple =      { 0x007f, 0x0000, 0x00ff };
    const RGB_Color Violet =      { 0x00ff, 0x0000, 0x00ff };
    const RGB_Color Magenta =     { 0x00ff, 0x0000, 0x0080 };
    const RGB_Color Indigo =      { 0x002b, 0x0006, 0x007f };
    const RGB_Color SoftRed =     { 0x0001, 0x0000, 0x0000 };
    const RGB_Color RoyalPurple = { 0x008a, 0x0006, 0x00cf };
};

const Pixel colorPalette[]  __attribute__((unused)) = {
  Colors::Black,
  Colors::White, // white
  Colors::Red, // red
  Colors::Orange, // orange
  Colors::Yellow, // yellow
  Colors::Chartreuse, // chartreuse
  Colors::Green, // green
  Colors::SpringGreen, // spring green
  Colors::Cyan, // cyan
  Colors::DodgerBlue, // dodger blue
  Colors::Blue, // blue
  Colors::Purple, // purple
  Colors::Violet, // violet
  Colors::Magenta, // magenta
};

const Pixel colorsExceptBlack[]  __attribute__((unused)) = {
  Colors::White, // white
  Colors::Red, // red
  Colors::Orange, // orange
  Colors::Yellow, // yellow
  Colors::Chartreuse, // chartreuse
  Colors::Green, // green
  Colors::SpringGreen, // spring green
  Colors::Cyan, // cyan
  Colors::DodgerBlue, // dodger blue
  Colors::Blue, // blue
  Colors::Purple, // purple
  Colors::Violet, // violet
  Colors::Magenta, // magenta
  Colors::RoyalPurple,
};

const Pixel rgbPalette[] __attribute__((unused)) = {
    Colors::Red,
    Colors::Green,
    Colors::Blue
};

const Pixel rainbow7[] __attribute__((unused)) = {
  Colors::Red,
  Colors::Orange,
  Colors::Yellow,
  Colors::Green,
  Colors::Blue,
  Colors::Indigo,
  Colors::Violet,
};

const Pixel rainbow12[] __attribute__((unused)) = {
  Colors::Red,
  Colors::Orange,
  Colors::Yellow,
  Colors::Chartreuse,
  Colors::Green ,
  Colors::SpringGreen,
  Colors::Cyan,
  Colors::DodgerBlue,
  Colors::Blue,
  Colors::Purple,
  Colors::Violet,
  Colors::Magenta,
};

#endif //ifndef __ANIMATION_LIB_H
