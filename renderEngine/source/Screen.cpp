#include "Screen.h"
#include "string.h"

Screen::Screen(int w, int h)
{
  width = w;
  height = h;

  m_depth = 0;
  while (1 << m_depth < w)
    m_depth++;
  // Create a new bitmap.
  image = new Image(m_depth, height);
}


