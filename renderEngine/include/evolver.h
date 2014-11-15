#pragma once
#include "basics.h"
#include "Thread.h"
#include "ScreenColour.h"
#include "Timer.h"

class Evolver : public Thread
{
public:
  static const int depth = 8;
  int frame;
  int type;
  Timer timer;

  Evolver(int type, bool bigSize, bool isDynamic);
  void load(char* fileName, int type);

  bool endUpdate;

  void randomiseMasks(const Evolver& master, float percentVariation);
  void randomise();
  void set(const Evolver& evolver, bool copyGrid = false);
  void draw();
  void update();
  void reset();
  void write(FILE* fp);
  void run(); 
  void setDynamic(bool dynamic)
  {
    isDynamic = dynamic;
    float renderWidth = (float)(1<<depthUsed);
    if (dynamic)
    {
      renderWidth *= 0.75; // since depth is halfed, we need more fog
      depthUsed = sizeAddition + depth-1;
      bitmap = smallBitmapData[bitmapIndex];
    }
    else
    {
      depthUsed = sizeAddition + depth;
      bitmap = largeBitmapData[bitmapIndex];
    }
    fogScale = 1.5f * 0.64f/renderWidth;  
  }
  bool getDynamic(){ return isDynamic; }

  void setToLetter(char letter){ this->letter = letter; }
  unsigned char* grids[depth+2];
  Colour normalColours[7];

  static const int numTypes = 10;
  // function callbacks:
	typedef bool (Evolver::*GetStaticValueFunction)(int level, int parentLevel, int x, int y, int z);
	typedef bool (Evolver::*GetDynamicValueFunction)(int level, int x, int y, int z, int numNeighbours);
  GetStaticValueFunction getStaticValue[numTypes];
  GetDynamicValueFunction getDynamicValue[numTypes];
private:
  bool isDynamic;
  class Image* largeBitmapData[2];
  class Image* smallBitmapData[2];
  class Image* bitmap;
  int bitmapIndex;
  bool drawLocked;
  int dynamicMinLevel; // used for fixed low res images in dynamic case


  void read(FILE* fp);
  void constrainLayer(int level);
  void renderPoint(int level, int x, int y, int z, int totalNeighbours);
  void renderToBitmap(int level, int x, int y, int z, int totalNeighbours);
  void updateStatic();
  void updateStaticFast(int level, int x, int y, int z, int targetDepth);
  unsigned short *zDepth; // z buffer
  bool *diagonalSet; // used for shadows
  char letter;
  int depthUsed;
  float fogScale;

  int scaleZ[depth+2];
  int scaleY[depth+2];
  int maxSize[depth+2];
  static const int fixedLevel = 2; // where shape is user specified or random

  // TODO: fix to be z major
  inline bool get(int level, int x, int y, int z) const
  {
    x++;
    y++;
    z+=2;
    unsigned char* contains = grids[level];
    int xDiv = x>>3; // /8
    ASSERT(x>=0 && xDiv<scaleY[level] && y >=0 && z>=0);
    ASSERT((xDiv + y*scaleY[level] + z*scaleZ[level]) < maxSize[level]);
    int xShift = x&7;
    const unsigned char& res = contains[xDiv + y*scaleY[level] + z*scaleZ[level]]; // unfortunately we can't use [] notation as the widths are not compile-time
    return (res & 1<<xShift) != 0;
  }

  // these two functions are grouped together. They make the update 10% faster, so not a big deal really
  unsigned char* setPointer;
  void initialiseSetFast(int level, int y, int z)
  {
    y++;
    z+=2;
    unsigned char* contains = grids[level];
    ASSERT(y >=0 && z>=0);
    setPointer = &contains[ y*scaleY[level] + z*scaleZ[level] ]; 
  }
  inline void setFast(int x, bool val)
  {
    x++;
    int xDiv = x>>3; // /8
//    ASSERT(x>=0 && xDiv<scaleY[level]);
    int xShift = x&7;
    unsigned char& res = setPointer[xDiv]; 
    if (val)
      res |= 1<<xShift;
    else
      res &= ~(1<<xShift);
  }

  inline void set(int level, int x, int y, int z, bool val)
  {
    x++;
    y++;
    z+=2;
    unsigned char* contains = grids[level];
    int xDiv = x>>3; // /8
    ASSERT(x>=0 && xDiv<scaleY[level] && y >=0 && z>=0);
    ASSERT((xDiv + y*scaleY[level] + z*scaleZ[level]) < maxSize[level]);
    int xShift = x&7;
    unsigned char& res = contains[xDiv + y*scaleY[level] + z*scaleZ[level]]; // unfortunately we can't use [] notation as the widths are not compile-time
    if (val)
      res |= 1<<xShift;
    else
      res &= ~(1<<xShift);
  }
//     inline void setTest(int level, int x, int y, int z, bool val)
//   {
//     x++;
//     y++;
//     z+=2;
//     unsigned char* contains = grids[level];
//     int xDiv = x>>3; // /8
//     ASSERT(x>=0 && xDiv<scaleY[level] && y >=0 && z>=0);
//     ASSERT((xDiv + y*scaleY[level] + z*scaleZ[level]) < maxSize[level]);
//     int xShift = x&7;
//     unsigned char& res = contains[xDiv + y*scaleY[level] + z*scaleZ[level]]; // unfortunately we can't use [] notation as the widths are not compile-time
//     unsigned char oldVal = res;
//     if (val)
//       res |= 1<<xShift;
//     else
//       res &= ~(1<<xShift);
//     ASSERT(res == oldVal);
//   }

  // Note, you must check level != depth before calling this
  inline int getNumInQuad(int level, int x, int y, int z, unsigned char* grid)
  {
    x++;
    y++;
    z+=2;
    int offsetY = scaleY[level];
    int offsetY2 = offsetY+offsetY;
    int offsetZ = scaleZ[level];
    int offset = y*offsetY + z*offsetZ;
    unsigned char* contains = grid + offset;

    int num = 0;
    num += getNumInDouble(contains, x);
    num += getNumInDouble(contains + offsetY, x);
    contains += offsetZ;
    num += getNumInDouble(contains, x);
    num += getNumInDouble(contains + offsetY, x);
    return num;
  }

  // Fast version of calling get 27 times, for one thing it unrolls the loop
  // the overall improvement is about 20% faster I think
  inline int getNumNeighbours(int level, int x, int y, int z, unsigned char* grid)
  {
    z+=1;
    int offsetY = scaleY[level];
    int offsetY2 = offsetY+offsetY;
    int offsetZ = scaleZ[level];
    int offset = y*offsetY + z*offsetZ;
    unsigned char* contains = grid + offset;

    int num = 0;
    int xShift1 = x&7;
    int xDiv1 = x>>3;
    x++;
    int xShift2 = x&7;
    int xDiv2 = x>>3;
    x++;
    int xShift3 = x&7;
    int xDiv3 = x>>3;
    addNumInLine(contains, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    addNumInLine(contains + offsetY, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    addNumInLine(contains + offsetY2, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    contains += offsetZ;
    addNumInLine(contains, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    addNumInLine(contains + offsetY, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    addNumInLine(contains + offsetY2, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    contains += offsetZ;
    addNumInLine(contains, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    addNumInLine(contains + offsetY, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    addNumInLine(contains + offsetY2, x, xDiv1, xDiv2, xDiv3, xShift1, xShift2, xShift3, num);
    return num;
  }

  // these two functions are grouped together, they make empty input much faster
  unsigned char *mat1, *mat2, *mat3, *mat4, *mat5, *mat6, *mat7, *mat8, *mat9;
  void initialiseNeighbourPlane(int level, int y, int z)
  {
    z+=1;
    int offsetY = scaleY[level];
    int offsetY2 = offsetY+offsetY;
    int offsetZ = scaleZ[level];
    int offset = y*offsetY + z*offsetZ;
    unsigned char* contains = grids[level] + offset;

    mat1 = contains;
    mat2 = contains + offsetY;
    mat3 = contains + offsetY2;
    contains += offsetZ;
    mat4 = contains;
    mat5 = contains + offsetY;
    mat6 = contains + offsetY2;
    contains += offsetZ;
    mat7 = contains;
    mat8 = contains + offsetY;
    mat9 = contains + offsetY2;
  }
  inline int getNeighbourPlane(int x)
  {
    int num = 0;
    int xShift1 = x&7;
    int xDiv1 = x>>3;

    num += (*(mat1 + xDiv1)>>xShift1)&1;
    num += (*(mat2 + xDiv1)>>xShift1)&1;
    num += (*(mat3 + xDiv1)>>xShift1)&1;
    num += (*(mat4 + xDiv1)>>xShift1)&1;
    num += (*(mat5 + xDiv1)>>xShift1)&1;
    num += (*(mat6 + xDiv1)>>xShift1)&1;
    num += (*(mat7 + xDiv1)>>xShift1)&1;
    num += (*(mat8 + xDiv1)>>xShift1)&1;
    num += (*(mat9 + xDiv1)>>xShift1)&1;
    return num;
  }
  inline int getNumInDouble(unsigned char* contains, int x)
  {
    int num = 0;
    int xDiv = x>>3; 
    int xShift = x&7;
    num += (contains[xDiv]>>xShift)&1;
    x++;
    xDiv = x>>3; 
    xShift = x&7;
    num += (contains[xDiv]>>xShift)&1;
    return num;
  }
  __forceinline void addNumInLine(unsigned char* contains, int x, int xDiv1, int xDiv2, int xDiv3, int xShift1, int xShift2, int xShift3, int& num)
  {
    num += (contains[xDiv1]>>xShift1)&1;
    num += (contains[xDiv2]>>xShift2)&1;
    num += (contains[xDiv3]>>xShift3)&1;
  }
  inline void setDistance(int x, int y, unsigned short bufferDepth)
  {
    int index = x + (y << depthUsed); 
    zDepth[index] = bufferDepth;
  }
  inline unsigned short getDistance(int x, int y)
  {
    return zDepth[x + (y << depthUsed)];
  }
  int sizeAddition; // for full view
  int yUp;

  // genomes
  static const int genomeSize = 1024;
  bool genome[genomeSize];

  bool getStatic1(int level, int parentLevel, int x, int y, int z);
  bool getStatic2(int level, int parentLevel, int x, int y, int z);
  bool getStatic3(int level, int parentLevel, int x, int y, int z);
  bool getStatic4(int level, int parentLevel, int x, int y, int z);
  bool getStatic5(int level, int parentLevel, int x, int y, int z);
  bool getStatic6(int level, int parentLevel, int x, int y, int z);
  bool getStatic7(int level, int parentLevel, int x, int y, int z);
  bool getStatic8(int level, int parentLevel, int x, int y, int z);
  bool getStatic9(int level, int parentLevel, int x, int y, int z);
  bool getDynamic1(int level, int x, int y, int z, int numNeighbours);
  bool getDynamic2(int level, int x, int y, int z, int numNeighbours);
  bool getDynamic3(int level, int x, int y, int z, int numNeighbours);
  bool getDynamic4(int level, int x, int y, int z, int numNeighbours);
  bool getDynamic5(int level, int x, int y, int z, int numNeighbours);
  bool getDynamic6(int level, int x, int y, int z, int numNeighbours);
  bool getDynamic7(int level, int x, int y, int z, int numNeighbours);
};
