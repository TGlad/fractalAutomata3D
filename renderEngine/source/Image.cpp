#include "Image.h"
#include "glut.h"
#include "string.h"

Image::Image(int width, int height)
{
  this->width = width;
  this->height = height;
  data = new ScreenColour[width * height];
}

Image::~Image(void)
{
}

void Image::draw()
{
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte *)data);
}

void Image::clear(char shade)
{
  memset(data, shade, width*height*4);
}