#include "Evolver.h"
#include "Image.h"
#include <memory>

static bool letterZ[4][4][4] = {
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
  {{1,1,1,1},{0,1,0,0},{0,0,1,0},{1,1,1,1}},
  {{1,1,1,1},{0,1,0,0},{0,0,1,0},{1,1,1,1}},
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};

static bool letterX[4][4][4] = {
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
  {{1,0,1,0},{0,1,0,0},{1,0,1,0},{0,0,0,0}},
  {{1,0,1,0},{0,1,0,0},{1,0,1,0},{0,0,0,0}},
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};

static bool letterC[4][4][4] = {
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
  {{0,0,1,1},{0,1,0,0},{0,1,0,0},{0,0,1,1}},
  {{0,0,1,1},{0,1,0,0},{0,1,0,0},{0,0,1,1}},
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};

static bool letterO[4][4][4] = {
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
  {{0,1,1,0},{1,0,0,1},{1,0,0,1},{0,1,1,0}},
  {{0,1,1,0},{1,0,0,1},{1,0,0,1},{0,1,1,0}},
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};

static bool letterDot[4][4][4] = {
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
  {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
  {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
  {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}};

static int depthDiv = 2; // proportional to time

Evolver::Evolver(int type, bool bigSize, bool isDynamic)
{
  sizeAddition = bigSize ? 1 : 0;
  depthUsed = depth+sizeAddition;
  yUp = 0;

  int largeSize = 1<<depthUsed;
  largeBitmapData[0] = new Image(largeSize, largeSize); // store each pixel
  largeBitmapData[1] = new Image(largeSize, largeSize); // store each pixel
  largeBitmapData[0]->clear(64);
  largeBitmapData[1]->clear(64);

  int smallSize = 1<<(depthUsed-1);
  smallBitmapData[0] = new Image(smallSize, smallSize); // store each pixel
  smallBitmapData[1] = new Image(smallSize, smallSize); // store each pixel
  smallBitmapData[0]->clear(64);
  smallBitmapData[1]->clear(64);

  bitmapIndex = 0;
  dynamicMinLevel = 1; // maximum depth by default
  bitmap = isDynamic ? smallBitmapData[bitmapIndex] : largeBitmapData[bitmapIndex];
  letter = 'r'; // for random
  drawLocked = false;
  endUpdate = false;

  for (int i = 0; i<=depthUsed; i++)
  {
    int w = (1<<i) + 2;
    int size = w * (w+1) * (1 + w/8);
    grids[i] = new unsigned char[size];
    memset(grids[i], 0, size);
    scaleY[i] = ((1<<i)+7+2)/8;
    scaleZ[i] = scaleY[i]*((1<<i)+2);
    maxSize[i] = size;
  }
  zDepth = new unsigned short[1<<(depthUsed + depthUsed)]; 
  diagonalSet = new bool[1<<(depthUsed + depthUsed + 1)]; 
  // set precalculated normal colours- 
  for (int i = 0; i<7; i++)
  {
    // I could (probably should) do a simple dot product with the light direction, but using this slightly modified linear
    // gives a more noticeable range of shades, which is nice when the lighting model is quite basic and it is 
    // important to show the '3d'ness.
    int I = i==6 ? 4: i; // i==5 is the 45 degree light direction.
    float normalBlend = (float)(I) / 5.0f; // 0 to 1
    normalColours[i] = Colour(1.0f, 0.85f, 0.4f)*normalBlend + Colour(0.2f, 0.1f, 0.1f)*2.0f*(1.0f - normalBlend);
  }
  
  
  setDynamic(isDynamic);
  this->type = type;
  for (int i = 0; i<numTypes; i++)
  {
    getDynamicValue[i] = NULL;
    getStaticValue[i] = NULL; 
  }
  getStaticValue[1] = &Evolver::getStatic1;
  getStaticValue[2] = &Evolver::getStatic2;
  getStaticValue[3] = &Evolver::getStatic3;
  getStaticValue[4] = &Evolver::getStatic4;
  getStaticValue[5] = &Evolver::getStatic5;
  getStaticValue[6] = &Evolver::getStatic6;
  getStaticValue[7] = &Evolver::getStatic7;
  getStaticValue[8] = &Evolver::getStatic8;
  getStaticValue[9] = &Evolver::getStatic9;
  getDynamicValue[1] = &Evolver::getDynamic1;
  getDynamicValue[2] = &Evolver::getDynamic2;
  getDynamicValue[3] = &Evolver::getDynamic3;
  getDynamicValue[4] = &Evolver::getDynamic4;
  getDynamicValue[5] = &Evolver::getDynamic5;
  getDynamicValue[6] = &Evolver::getDynamic6;
  getDynamicValue[7] = &Evolver::getDynamic7;
  reset();
}

void Evolver::reset()
{
  randomise();
  randomiseMasks(*this, 50.0f); // 50%
}

void Evolver::load(char* fileName, int type)
{
  this->type = type;
  setDynamic(isDynamic);
  FILE* fp;
  if (fopen_s(&fp, fileName, "rb"))
  {
    printf("Cannot find file: %s\n", fileName);
    return;
  }
  read(fp);
  fclose(fp);
}

void Evolver::randomise()
{
  frame = 1;
  int size = 1<<fixedLevel;
  dynamicMinLevel = fixedLevel+1;
  
  memset(grids[fixedLevel], 0, sizeof(char) * maxSize[fixedLevel]);
  switch (letter) // a few basic examples
  {
    case 'z':
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          set(fixedLevel, x, y, z, letterZ[z][y][x]);
    break;
    case 'x':
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          set(fixedLevel, x, y, z, letterX[z][y][x]);
    break;
    case 'c':
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          set(fixedLevel, x, y, z, letterC[z][y][x]);
    break;
    case '.':
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          set(fixedLevel, x, y, z, letterDot[z][y][x]);
    break;
    case 'o':
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          set(fixedLevel, x, y, z, letterO[z][y][x]);
    break;
    case 'h': // hills (top down view)
    yUp = 0;
    // set backdrop to on
    for (int z = 0; z<size+1; z++)
      for (int y = -1; y<size+1; y++)
        for (int x = -1; x<size+1; x++)
          set(fixedLevel, x, y, z, z >= 2);
    // random pertubation of hills
    for (int i = 0; i<(size*size / 4); i++)
    {
      set(fixedLevel, rand()%size, rand()%size, 2, false);
      set(fixedLevel, rand()%size, rand()%size, 1, true);
    }
    break;
    case 'g': // ground
    yUp = 2;
    // set backdrop to on
    for (int z = -1; z<size+1; z++)
      for (int y = 0; y<size+1; y++)
        for (int x = -1; x<size+1; x++)
          set(fixedLevel, x, y, z, y >= 2);
    // random pertubation of hills
    for (int i = 0; i<(size*size / 4); i++)
    {
      set(fixedLevel, rand()%size, 2, rand()%size, false);
      set(fixedLevel, rand()%size, 1, rand()%size, true);
    }
    break;
    case 'r':
    {
      dynamicMinLevel = 1;
      int count = (size*size*size)/2;
      memset(grids[fixedLevel], 0, maxSize[fixedLevel]);
      for (int i = 0; i<count; i++)
        set(fixedLevel, rand()&3, rand()&3, rand()&3, true);
    }
  }
  if (isDynamic)
  {
    // initialise the bitmaps to some random image:
    // This is a recursive process, generating the data procedurally as we go deeper in detail level.
    set(0, 0, 0, 0, true); // the base level has to be on if you think about it, otherwise the whole volume would be culled
    for (int level = dynamicMinLevel; level<=depthUsed; level++)
    {
      memset(grids[level], 0, maxSize[level]);
      int size = 1<<level;
      int halfsize = size/depthDiv;
      for (int k = 0; k<halfsize; k++)
        for (int j = 0; j<size; j++)
          for (int i = 0; i<size; i++)
            if (random() > 0.75f)
              set(level, i, j, k, true);
    }
  }
}

void Evolver::randomiseMasks(const Evolver& master, float percentVariation)
{
  type = master.type;
  yUp = master.yUp;
  setDynamic(master.isDynamic);
  float threshold = 1.0f - 2.0f*0.01f*percentVariation;

  for (int i = 0; i<genomeSize; i++)
    if (random() > threshold)
      genome[i] = !genome[i];
}

void Evolver::read(FILE* fp)
{
  fread(genome, sizeof(bool), genomeSize, fp);
}

void Evolver::write(FILE* fp)
{
  fwrite(genome, sizeof(bool), genomeSize, fp);
}

void Evolver::set(const Evolver& evolver, bool copyGrid)
{
  type = evolver.type;
  yUp = evolver.yUp;
  setDynamic(evolver.isDynamic);

  memcpy(genome,  evolver.genome,  sizeof(bool)*genomeSize);

  if (copyGrid && !isDynamic)
  {
    int size = 1<<fixedLevel;
    memcpy(grids[fixedLevel], evolver.grids[fixedLevel], maxSize[fixedLevel]);
  }
}


void Evolver::draw()
{
  drawLocked = true;
  if (isDynamic)
  {
 //   glPixelZoom(1.0f, 1.0f); // so we have more speed to play with
    glPixelZoom(2.0f, 2.0f); // so we have more speed to play with
    smallBitmapData[1-bitmapIndex]->draw(); // draw the opposite to what is being rendered
  }
  else
  {
    glPixelZoom(1.0f, 1.0f); 
    largeBitmapData[1-bitmapIndex]->draw(); // draw the opposite to what is being rendered
  }
  drawLocked = false; // mutex
}


// take the stacked grid data and generate a single image from it
void Evolver::updateStaticFast(int level, int x, int y, int z, int targetDepth)
{
  if (level == targetDepth-1) 
  {
    level++;
    x = x<<1;
    y = y<<1; 
    z = z<<1;
    for (int k = 0; k<2; k++)
      for (int j = 0; j<2; j++)
        for (int i = 0; i<2; i++) 
          set(level, x+i, y+j, z+k, (this->*getStaticValue[type])(level, level-1, x+i, y+j, z+k));
    return;
  }
  // If it cannot contain a surface because it and all neighbours are either empty or full, then early-out:
  int numNeighbours = getNumNeighbours(level, x, y, z, grids[level]);
  if (numNeighbours == 27 || numNeighbours == 0)
    return;

  level++;
  x = x<<1;
  y = y<<1; 
  z = z<<1;
  // Call render on children
  for (int k = 0; k<2; k++)
    for (int j = 0; j<2; j++)
      for (int i = 0; i<2; i++) 
        updateStaticFast(level, x+i, y+j, z+k, targetDepth);
}

void Evolver::updateStatic()
{
//  timer.start();
  int size = 1<<fixedLevel;

#define UPDATE_FAST
#if defined(UPDATE_FAST)
  for (int targetDepth = fixedLevel+1; targetDepth<=depthUsed; targetDepth++)
  {
    memset(grids[targetDepth], 0, sizeof(unsigned char) * maxSize[targetDepth]);
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          updateStaticFast(fixedLevel, x, y, z, targetDepth);
  }
#else
  GetStaticValueFunction getValue = getStaticValue[type];
  for (int i = fixedLevel+1; i<=depthUsed; i++)
  {
    int size = 1<<i;
    for (int z = 0; z<size; z++)
      for (int y = 0; y<size; y++)
        for (int x = 0; x<size; x++)
          set(i, x, y, z, (this->*getValue)(i, i-1, x, y, z));
  }
#endif
//   timer.stop();
//   timer.print("Update static time\n");
//   timer.reset();
}

void Evolver::renderPoint(int level, int x, int y, int z, int totalNeighbours)
{
  float shade = 1.0f;
#define SHADOWS
#if defined(SHADOWS)
  int height = z + (1<<depthUsed) - y;
  bool *shadow = diagonalSet + (height<<depthUsed) + x;
  if (*shadow)
    shade *= 0.5f; // in shadow
  else
    *shadow = true;
#endif
  if (getDistance(x, y) < z)
    return;
  Colour col(0.7f, 0.7f, 0.7f);
#define DIFFUSE_SHADING
#if defined(DIFFUSE_SHADING)
  int d1 = -1;
  while (d1 < 2 && !get(level, x, y+1, z+d1))
    d1++;
  int d2 = -1;
  while (d2 < 2 && !get(level, x, y-1, z+d2))
    d2++;
  col = normalColours[3 + d2 - d1];
#endif

//#define AMBIENT_OCCLUSION
#if defined(AMBIENT_OCCLUSION)
  float occlusionShade = 1.0f - ((float)totalNeighbours / (float)(27*depthUsed));
  ASSERT(occlusionShade >= 0.0f && occlusionShade <= 1.0f);
  shade *= occlusionShade;
#endif
  col *= shade;

#define FOG
#if defined(FOG)
  Weight blend = (float)z * fogScale;
  blend = blend > 1.0f ? 1.0f : blend;

  // now check for shadows
  col *= (1.0f - blend);
  col += Vector3(0.25f, 0.25f, 0.25f)*blend;
#endif


  // Note, we might want to blur in the case of large pixels
  ScreenColour colour(col);
  
  bitmap->setPixel(x, y, colour);
  setDistance(x, y, z);
}

// take the stacked grid data and generate a single image from it
void Evolver::renderToBitmap(int level, int x, int y, int z, int totalNeighbours)
{
  // If at highest level then render
  if (level == depthUsed-1) 
  {
    level++;
    x = x<<1;
    y = y<<1;
    z = z<<1;
    for (int k = 0; k<2; k++)
    {
      int Z = z+k;
      for (int j = 0; j<2; j++)
      {
        int Y = y+j;
        for (int i = 0; i<2; i++) 
        {
          int X = x + i;
          if (get(level, X, Y, Z))
            renderPoint(level, X, Y, Z, totalNeighbours);
        }
      }
    }
    return;
  }
  
  // If it cannot contain a surface because it and all neighbours are either empty or full, then early-out:
  int numNeighbours = getNumNeighbours(level, x, y, z, grids[level]);
  if (numNeighbours == 27 || numNeighbours == 0)
    return;

  // Call render on children
  level++;
  x = x<<1;
  y = y<<1;
  z = z<<1;
  for (int k = 0; k<2; k++)
  {
    for (int j = 0; j<2; j++)
    {
      for (int i = 0; i<2; i++) 
      {
        renderToBitmap(level, x+i, y+j, z+k, totalNeighbours + numNeighbours);
      }
    }
  }
}
void Evolver::run() // does it on a separate thread
{
  while (!endUpdate)
  {
    update();
  }
  endUpdate = false;
}

void Evolver::update()
{
  // This performs the covolution
  if (!isDynamic)
  {
    if (frame == 1)
    {
      memset(zDepth, 255, sizeof(unsigned short)<<(depthUsed + depthUsed));
      memset(diagonalSet, 0, sizeof(bool)<<(depthUsed + 1 + depthUsed)); // clear shadow buffer
      bitmap->clear(64);

      updateStatic();
      int size = 1<<fixedLevel;
      for (int k = 0; k<size; k++)
      {
        for (int j = 0; j<size; j++)
        {
          for (int i = 0; i<size; i++)
          {
            renderToBitmap(fixedLevel, i, j, k, 0);
          }
        }
      }
      while (drawLocked)
      {
        bitmapIndex = bitmapIndex * 1;
      }
      bitmapIndex = 1-bitmapIndex;
      bitmap = largeBitmapData[bitmapIndex];
    }
    frame++;
    endUpdate = true;
    return;
  }

  // I need to get the timing algorithm right here...
  int currentLevel = -1;
  for (int i = 0; i<32 && currentLevel==-1; i++)
    if ((1<<i) & frame)
      currentLevel = i; // should go 0,1,0,2,0,1,0,3,...
  currentLevel = depthUsed - currentLevel; // so most common is the highest detail.
  if (currentLevel < dynamicMinLevel)
  {
    frame = 1;
    return;
  }
  ASSERT(currentLevel <= depthUsed && currentLevel >= dynamicMinLevel);

  frame++; // now on correct frame for updating and drawing
  int size = 1<<currentLevel;
  bool isDrawLevel = currentLevel == depthUsed;
  if (isDrawLevel) // do two updates per frame, since one is small, this keeps the update about even cost
  {
 //   timer.start();
    memset(zDepth, 255, sizeof(unsigned short)<<(depthUsed + depthUsed));
    memset(diagonalSet, 0, sizeof(bool)<<(depthUsed + 1 + depthUsed)); // clear shadow buffer
    bitmap->clear(64);
  }
  int halfsize = size/depthDiv;
  for (int k = 0; k<halfsize && !endUpdate; k++)
  {
    for (int j = 0; j<size; j++)
    {
      initialiseNeighbourPlane(currentLevel, j, k);
      int num1 = 0;
      int num2 = getNeighbourPlane(0);
      int num3 = getNeighbourPlane(1);
      initialiseSetFast(currentLevel, j, k-2);
      for (int i = 0; i<size; i++) // basic scheme
      {
        num1 = num2;
        num2 = num3;
        num3 = getNeighbourPlane(i+2);
        int numNeighbours = num1+num2+num3;
 //       ASSERT(numNeighbours == getNumNeighbours(currentLevel, i, j, k, grids[currentLevel]));
        bool b = numNeighbours == 0 ? false : (this->*getDynamicValue[type])(currentLevel, i, j, k, numNeighbours);
        setFast(i, b);
 //       setTest(currentLevel, i, j, k-2, b);
        if (b && isDrawLevel)
          renderPoint(currentLevel, i, j, k, 0); // rendering right then is slightly faster... perhaps 15%
      }
    }
  }
  memmove(grids[currentLevel] + scaleZ[currentLevel]*2, grids[currentLevel], scaleZ[currentLevel]*halfsize);
  if (isDrawLevel)
  {
    while (drawLocked)
    {
      bitmapIndex = bitmapIndex * 1;
    }
    bitmapIndex = 1-bitmapIndex;
    bitmap = smallBitmapData[bitmapIndex];

//     timer.stop();
//     timer.print("Update and draw slow\n");
//     timer.reset();
  }
}
