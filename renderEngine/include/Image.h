#pragma once
#include "basics.h"
#include "ScreenColour.h"

class Image
{
public:
  int width;
  int height;
  int widthShift;
  ScreenColour *data;

  inline void setPixel(int x, int y, const ScreenColour& colour)
  {
    ScreenColour *ptr = data + x + (width-1 - y)*width;
    *ptr = colour;
  }
  void draw();
  void clear(char shade = 0);

  Image(int width, int height);
  ~Image(void);
};
