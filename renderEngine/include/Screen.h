#pragma once
#include "basics.h"
#include "ScreenColour.h"
#include "Image.h"

class Screen
{
public:
  int width;
  int height;
  int m_depth; // just log2 of the width
  Image *image;

  Screen(int w, int h);
  ~Screen();
};