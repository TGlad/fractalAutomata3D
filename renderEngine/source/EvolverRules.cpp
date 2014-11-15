#include "Evolver.h"
#include <memory.h>
// Code related to specific rule types

// type 1 
bool Evolver::getStatic1(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int numParents = getNumInQuad(parentLevel, xParent - ((x+1)&1), yParent - ((y+1)&1), zParent - ((z+1)&1), grids[parentLevel])<<3;
  if (numParents == 0)
    return false;
  if (numParents == 64)
    return true;
  if (get(parentLevel, xParent, yParent, zParent))
    numParents += 3*8; // so 5 points for centre voxel

  for (int kk = 0; kk<2; kk++)
  {
    for (int jj = 0; jj<2; jj++)
    {
      numParents += 4*(int)get(parentLevel, xParent-dirX, yParent+jj*dirY, zParent+kk*dirZ);
      numParents += 4*(int)get(parentLevel, xParent+jj*dirX, yParent-dirY, zParent+kk*dirZ);
      numParents += 4*(int)get(parentLevel, xParent+jj*dirX, yParent+kk*dirY, zParent-dirZ);
    }
    numParents += 2*(int)get(parentLevel, xParent+kk*dirX, yParent-dirY, zParent-dirZ);
    numParents += 2*(int)get(parentLevel, xParent-dirX, yParent+kk*dirY, zParent-dirZ);
    numParents += 2*(int)get(parentLevel, xParent-dirX, yParent-dirY, zParent+kk*dirZ);
  }

  int total = (numParents + 2)>>3; // rounded to nearest. 0 to 19
  if (total <= 3)
    return false;
  else if (total >= 16)
    return true;
  if (total < 10)
    return genome[total];
  else 
    return !genome[15-total];
}

// type 1
bool Evolver::getStatic2(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int second = (int)get(parentLevel, xParent+dirX, yParent, zParent) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent, yParent, zParent+dirZ);
  int third =  (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent+dirX, yParent, zParent+dirZ) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent+dirZ);
  int fourth = (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent+dirZ);
  int numParents = nearest + second + third + fourth;
  // Note- these two are mandatory
  if (numParents <= 2)
    return false;
  else if (numParents >= 6)
    return true;

  return genome[nearest*32 + second*8 + third*4 + fourth];
}

// type 1
bool Evolver::getStatic3(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int second = (int)get(parentLevel, xParent+dirX, yParent, zParent) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent, yParent, zParent+dirZ);
  int third =  (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent+dirX, yParent, zParent+dirZ) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent+dirZ);
  int fourth = get(parentLevel, xParent+dirX, yParent+dirY, zParent+dirZ)!=0;
  int numParents = nearest + second + third + fourth;
  // Note- these two are mandatory
  if (numParents <= 1)
    return false;
  else if (numParents >= 7)
    return true;
  
//   int sixth = (int)get(parentLevel, xParent-dirX, yParent+dirY, zParent) + 
//               (int)get(parentLevel, xParent-dirX, yParent, zParent+dirZ) + 
//               (int)get(parentLevel, xParent+dirX, yParent-dirY, zParent) + 
//               (int)get(parentLevel, xParent, yParent-dirY, zParent+dirZ) + 
//               (int)get(parentLevel, xParent+dirX, yParent, zParent-dirZ) +
//               (int)get(parentLevel, xParent, yParent+dirY, zParent-dirZ);
  int seventh = (int)get(parentLevel, xParent-dirX, yParent, zParent) + 
                (int)get(parentLevel, xParent, yParent-dirY, zParent) + 
                (int)get(parentLevel, xParent, yParent, zParent-dirZ);
  if (numParents + seventh <=3)
    return false;
  else if (numParents + seventh >= 8)
    return false;

  return genome[(nearest*32 + second*8 + third*4)*4 + seventh];
}


bool Evolver::getStatic4(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int second = (int)get(parentLevel, xParent+dirX, yParent, zParent) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent, yParent, zParent+dirZ);
  int third =  (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent+dirX, yParent, zParent+dirZ) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent+dirZ);
  int fourth = get(parentLevel, xParent+dirX, yParent+dirY, zParent+dirZ)!=0;
  int numParents = nearest + second + third + fourth;
  // Note- these two are mandatory
  if (numParents <= 1)
    return false;
  else if (numParents >= 7)
    return true;
  
  int fifth = (int)get(parentLevel, xParent-dirX, yParent+dirY, zParent+dirZ) + 
              (int)get(parentLevel, xParent+dirX, yParent-dirY, zParent+dirZ) + 
              (int)get(parentLevel, xParent+dirX, yParent+dirZ, zParent-dirZ);
  int sixth = (int)get(parentLevel, xParent-dirX, yParent+dirY, zParent) + 
              (int)get(parentLevel, xParent-dirX, yParent, zParent+dirZ) + 
              (int)get(parentLevel, xParent+dirX, yParent-dirY, zParent) + 
              (int)get(parentLevel, xParent, yParent-dirY, zParent+dirZ) + 
              (int)get(parentLevel, xParent+dirX, yParent, zParent-dirZ) +
              (int)get(parentLevel, xParent, yParent+dirY, zParent-dirZ);
  int seventh = (int)get(parentLevel, xParent-dirX, yParent, zParent) + 
                (int)get(parentLevel, xParent, yParent-dirY, zParent) + 
                (int)get(parentLevel, xParent, yParent, zParent-dirZ);
  int totalParents = numParents + fifth + sixth + seventh; // total = 20 (will never be actually 20, or 0)

  return genome[totalParents];
}

bool Evolver::getStatic5(int level, int parentLevel, int x, int y, int z)
{
  int dirX = (x%2) ? 1 : -1;
  int xParent = x>>1;

  int dirY = (y%2) ? 1 : -1;
  int yParent = y>>1;

  int dirZ = (z%2) ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent,      yParent,      zParent);
  int s1 = (int)get(parentLevel, xParent+dirX, yParent,      zParent);
  int s2 = (int)get(parentLevel, xParent,      yParent+dirY, zParent);
  int s3 = (int)get(parentLevel, xParent,      yParent,      zParent+dirZ);
  int second = s1 + s2 + s3;
  int t1 = (int)get(parentLevel, xParent,      yParent+dirY, zParent+dirZ);
  int t2 = (int)get(parentLevel, xParent+dirX, yParent,      zParent+dirZ);
  int t3 = (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent);
  int third = t1 + t2 + t3;
  
  int v1 = (int)get(parentLevel, xParent-dirX, yParent,      zParent);
  int v2 = (int)get(parentLevel, xParent,      yParent-dirY, zParent);
  int v3 = (int)get(parentLevel, xParent,      yParent,      zParent-dirZ);
  int seventh = v1 + v2 + v3;
  int totalParents = nearest + second + third + seventh; // total = 10
  if (totalParents <= 3)
    return false;
  if (totalParents >= 7)
    return true;

  int pattern1 = (s1<<0) + (s2<<1) + (s3<<2) + (t1<<3) + (t2<<4) + (t3<<5) + (v1<<6) + (v2<<7) + (v3<<8); 
  int pattern2 = (s2<<0) + (s3<<1) + (s1<<2) + (t2<<3) + (t3<<4) + (t1<<5) + (v2<<6) + (v3<<7) + (v1<<8); 
  int pattern3 = (s3<<0) + (s1<<1) + (s2<<2) + (t3<<3) + (t1<<4) + (t2<<5) + (v3<<6) + (v1<<7) + (v2<<8); 
  int pattern4 = (s1<<0) + (s3<<1) + (s2<<2) + (t1<<3) + (t3<<4) + (t2<<5) + (v1<<6) + (v3<<7) + (v2<<8); 
  int pattern5 = (s3<<0) + (s2<<1) + (s1<<2) + (t3<<3) + (t2<<4) + (t1<<5) + (v3<<6) + (v2<<7) + (v1<<8); 
  int pattern6 = (s2<<0) + (s1<<1) + (s3<<2) + (t2<<3) + (t1<<4) + (t3<<5) + (v2<<6) + (v1<<7) + (v3<<8); 
  int pattern = min(min(min(min(min(pattern1, pattern2), pattern3), pattern4), pattern5), pattern6);
  int total = nearest + 2*pattern; // max 1023
  ASSERT(total >= 0 && total <= 1023);
  if (total <= 511)
    return genome[total];
  else
    return !genome[1023 - total]; // bit symmetry
}

bool Evolver::getStatic6(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int s1 = (int)get(parentLevel, xParent+dirX, yParent, zParent);
  int s2 = (int)get(parentLevel, xParent, yParent+dirY, zParent);
  int s3 = (int)get(parentLevel, xParent, yParent, zParent+dirZ);
  int second = s1 + s2 + s3;
  int t1 = (int)get(parentLevel, xParent, yParent+dirY, zParent+dirZ);
  int t2 = (int)get(parentLevel, xParent+dirX, yParent, zParent+dirZ);
  int t3 = (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent);
  int third = t1 + t2 + t3;
  int fourth = get(parentLevel, xParent+dirX, yParent+dirY, zParent+dirZ)!=0;
  
  int v1 = (int)get(parentLevel, xParent-dirX, yParent, zParent);
  int v2 = (int)get(parentLevel, xParent, yParent-dirY, zParent);
  int v3 = (int)get(parentLevel, xParent, yParent, zParent-dirZ);
  int seventh = v1 + v2 + v3;
  int totalParents = nearest + second + third + fourth + seventh; // total = 11
  if (totalParents <= 3)
    return false;
  if (totalParents >= 8)
    return true;

  int pattern1 = (s1<<0) + (s2<<1) + (s3<<2) + (t1<<3) + (t2<<4) + (t3<<5) + (v1<<6) + (v2<<7) + (v3<<8); 
  int pattern2 = (s2<<0) + (s3<<1) + (s1<<2) + (t2<<3) + (t3<<4) + (t1<<5) + (v2<<6) + (v3<<7) + (v1<<8); 
  int pattern3 = (s3<<0) + (s1<<1) + (s2<<2) + (t3<<3) + (t1<<4) + (t2<<5) + (v3<<6) + (v1<<7) + (v2<<8); 
  int pattern4 = (s1<<0) + (s3<<1) + (s2<<2) + (t1<<3) + (t3<<4) + (t2<<5) + (v1<<6) + (v3<<7) + (v2<<8); 
  int pattern5 = (s3<<0) + (s2<<1) + (s1<<2) + (t3<<3) + (t2<<4) + (t1<<5) + (v3<<6) + (v2<<7) + (v1<<8); 
  int pattern6 = (s2<<0) + (s1<<1) + (s3<<2) + (t2<<3) + (t1<<4) + (t3<<5) + (v2<<6) + (v1<<7) + (v3<<8); 
  int pattern = min(min(min(min(min(pattern1, pattern2), pattern3), pattern4), pattern5), pattern6);
  int total = nearest + 2*fourth + 4*pattern; // max 2047
  if (total <= 1023)
    return genome[total];
  else
    return !genome[2047 - total]; // bit symmetry
}

// This one is asymmetric in height, which is the z axis
bool Evolver::getStatic7(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;
  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;
  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;
  int numParents = 0;

  // idea, use just the adjacent edges, so 7 voxels in total
  // then apply horizontal square symmetry and bit symmetry to get the numbers down
  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int a1 = (int)get(parentLevel, xParent+dirX, yParent, zParent);
  int a2 = (int)get(parentLevel, xParent, yParent+dirY, zParent);
  int a3 = (int)get(parentLevel, xParent, yParent, zParent+dirZ);
  int sum = nearest + a1 + a2 + a3;
  if (sum < 1)
    return false;
  if (sum > 3)
    return true;
  int s1 = (int)get(parentLevel, xParent-dirX, yParent, zParent);
  int s2 = (int)get(parentLevel, xParent, yParent-dirY, zParent);
  int s3 = (int)get(parentLevel, xParent, yParent, zParent-dirZ);
  int pattern1 = nearest + (a1<<1) + (a2<<2) + (a3<<3) + (s1<<4) + (s2<<5) + (s3<<6);
  int pattern2 = yUp ? nearest + (a3<<1) + (a2<<2) + (a1<<3) + (s3<<4) + (s2<<5) + (s1<<6)
                     : nearest + (a2<<1) + (a1<<2) + (a3<<3) + (s2<<4) + (s1<<5) + (s3<<6); // swapping 1 & 2 is a horizontal mirroring

  int total = min(pattern1, pattern2); // 0 to 127
  if (total < 64) // bit symmetry
    return genome[total];
  else
    return genome[127 - total];
}

// This one is asymmetric in height, which is the z axis
bool Evolver::getStatic8(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int second = (int)get(parentLevel, xParent+dirX, yParent, zParent) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent, yParent, zParent+dirZ);
  int third =  (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent+dirX, yParent, zParent+dirZ) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent+dirZ);
  int fourth = (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent+dirZ);
  int numParents = nearest + second + third + fourth;
  // Note- these two are mandatory
  if (numParents <= 2)
    return false;
  else if (numParents >= 6)
    return true;

  int total = nearest*32 + second*8 + third*4 + fourth; // total = 63
  int up = yUp ? y+yUp : z;
  if (total < 32)
    return genome[total + (up&1)*32];
  else
    return !genome[(63 - total) + (up&1)*32];
}

// This one is asymmetric in height, which is the z axis
bool Evolver::getStatic9(int level, int parentLevel, int x, int y, int z)
{
  int dirX = x%2 ? 1 : -1;
  int xParent = x>>1;

  int dirY = y%2 ? 1 : -1;
  int yParent = y>>1;

  int dirZ = z%2 ? 1 : -1;
  int zParent = z>>1;

  int nearest = (int)get(parentLevel, xParent, yParent, zParent);
  int second = yUp ? (int)get(parentLevel, xParent+dirX, yParent, zParent) + 
                     (int)get(parentLevel, xParent, yParent, zParent+dirZ)
                   : (int)get(parentLevel, xParent+dirX, yParent, zParent) + 
                     (int)get(parentLevel, xParent, yParent+dirY, zParent);
  int third =  (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent) + 
               (int)get(parentLevel, xParent+dirX, yParent, zParent+dirZ) + 
               (int)get(parentLevel, xParent, yParent+dirY, zParent+dirZ);
  int fourth = (int)get(parentLevel, xParent+dirX, yParent+dirY, zParent+dirZ);
  int v1 = (int)get(parentLevel, xParent-dirX, yParent, zParent);
  int v2 = (int)get(parentLevel, xParent, yParent-dirY, zParent);
  int v3 = (int)get(parentLevel, xParent, yParent, zParent-dirZ);
  int seventh = yUp ? v1 + v3 : v1 + v2;
  int numParents = nearest + second + third + fourth + seventh;
  // Note- these two are mandatory
  if (numParents <= 3)
    return false;
  else if (numParents >= 6)
    return true;

  int total = seventh*64 + nearest*32 + second*8 + third*4 + fourth; // total = 319
  int up = yUp ? y+yUp : z;
  if (total < 160)
    return genome[total + (up&1)*160];
  else
    return !genome[(319 - total) + (up&1)*160];
}
// this is an odd function (it acts the same if 0 and 1 are swapped
bool Evolver::getDynamic1(int level, int x, int y, int z, int numNeighbours)
{
  if (numNeighbours == 27)
    return true;
  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;

  bool self = get(level, x, y, z);
  numNeighbours -= (int)self;
  numNeighbours += level > 0 ? getNumInQuad(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ, grids[level-1]) : 4;
  numNeighbours += level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4;

  if (numNeighbours <= 8)
    return false;
  if (numNeighbours >= (42 - 8))
    return true;

  if (numNeighbours < 21)
    return genome[numNeighbours + 21*(int)self];
  else if (numNeighbours > 21)
    return !genome[42 - numNeighbours + 21*(1-(int)self)];
  return self; // if mid way then just return what centre currently is
}

// bit symmetric
bool Evolver::getDynamic2(int level, int x, int y, int z, int numNeighbours)
{
  if (numNeighbours == 27)
    return true;
  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;

  bool self = get(level, x, y, z);
  numNeighbours -= (int)self;
  int numParents = level > 0 ? getNumInQuad(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ, grids[level-1]) : 4;
  int numChildren = level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4;

  numNeighbours += (numChildren>>2) + 4*numParents; // total = 60, mid = 30
  if (numNeighbours < 8)
    return false;
  if (numNeighbours > (60 - 8))
    return true;

  if (numNeighbours < 30)
    return genome[numNeighbours];
  else if (numNeighbours > 30)
    return !genome[60-numNeighbours];
  else 
    return self; // equal in all respects so leave as it is
}


bool Evolver::getDynamic3(int level, int x, int y, int z, int numNeighbours)
{
  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;

  int numParents = level > 0 ? getNumInQuad(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ, grids[level-1]) : 4;
  int numChildren = level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4;

  numNeighbours = max(numNeighbours, max(numParents, numChildren));
  if (numNeighbours < 5)
    return false;

  return genome[numNeighbours]; // very simple rule here. TODO: add more rule types
}

// this is an odd function (it acts the same if 0 and 1 are swapped
bool Evolver::getDynamic4(int level, int x, int y, int z, int numNeighbours)
{
  if (numNeighbours == 27)
    return true;

  int numFaces = (int)get(level, x-1, y, z) + (int)get(level, x+1, y, z) + 
                 (int)get(level, x, y-1, z) + (int)get(level, x, y+1, z) + 
                 (int)get(level, x, y, z-1) + (int)get(level, x, y, z+1);
  int numChildren = level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4; 
  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;
  int numParents = level > 0 ? getNumInQuad(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ, grids[level-1]) : 4;
  if (numFaces < 1)
    numFaces = 1;
  else if (numFaces > 5)
    numFaces = 5;
  if (numChildren < 2)
    numChildren = 2;
  else if (numChildren > 6)
    numChildren = 6;
  if (numParents < 2)
    numParents = 2;
  else if (numParents > 6)
    numParents = 6;
  numFaces--;   // 0 to 4
  numChildren-=2;// 0 to 4
  numParents-=2; // 0 to 4

  int total = (numFaces*5 + numChildren)*5 + numParents; // max = 124
  if (total < 62)
    return genome[total]; // genome size 62
  else if (total > 62)
    return !genome[124 - total];
  else return get(level, x, y, z);
}


// this is an odd function (it acts the same if 0 and 1 are swapped
bool Evolver::getDynamic5(int level, int x, int y, int z, int numNeighbours)
{
  if (numNeighbours == 27)
    return true;

  bool self = get(level, x, y, z);
  int numFaces = (int)get(level, x-1, y, z) + (int)get(level, x+1, y, z) + 
                 (int)get(level, x, y-1, z) + (int)get(level, x, y+1, z) + 
                 (int)get(level, x, y, z-1) + (int)get(level, x, y, z+1) + 
                 2*(int)self;
  int numChildren = level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4; 
  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;
  int numParents = level > 0 ? getNumInQuad(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ, grids[level-1]) : 4;
  if (numFaces < 2)
    numFaces = 2;
  else if (numFaces > 6)
    numFaces = 6;
  if (numChildren < 2)
    numChildren = 2;
  else if (numChildren > 6)
    numChildren = 6;
  if (numParents < 2)
    numParents = 2;
  else if (numParents > 6)
    numParents = 6;
  numFaces-=2;   // 0 to 4
  numChildren-=2;// 0 to 4
  numParents-=2; // 0 to 4

  int total = (numFaces*5 + numChildren)*5 + numParents; // max = 124
  if (total < 62)
    return genome[total]; // genome size 62
  else if (total > 62)
    return !genome[124 - total];
  else return self;
}

bool Evolver::getDynamic6(int level, int x, int y, int z, int numNeighbours)
{
  if (numNeighbours == 27)
    return true;

  int numChildren = level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4; 
  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;
  int numParents = level > 0 ? getNumInQuad(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ, grids[level-1]) : 4;
  if (numNeighbours < 11)
    numNeighbours = 11;
  else if (numNeighbours > 16)
    numNeighbours = 16;
  if (numChildren < 2)
    numChildren = 2;
  else if (numChildren > 6)
    numChildren = 6;
  if (numParents < 2)
    numParents = 2;
  else if (numParents > 6)
    numParents = 6;
  numNeighbours-=11; // 0 to 5
  numChildren-=2;// 0 to 4
  numParents-=2; // 0 to 4

  int total = (numNeighbours*5 + numChildren)*5 + numParents; // max = 149
  if (total <= 74)
    return genome[total]; // genome size 74
  else 
    return !genome[149 - total];
}


bool Evolver::getDynamic7(int level, int x, int y, int z, int numNeighbours)
{
  if (numNeighbours == 27)
    return true;

  bool self = get(level, x, y, z);
  int numCentres = (int)self;
  numCentres += level>0 ? (int)get(level-1, x>>1, y>>1, z>>1) : 1;
  numCentres += level<depthUsed ? getNumInQuad(level+1, x<<1, y<<1, z<<1, grids[level+1]) : 4; 

  int dirX = (x+1)&1;
  int dirY = (y+1)&1;
  int dirZ = (z+1)&1;
  int numFaces = (int)get(level, x-1, y, z) + (int)get(level, x+1, y, z)
               + (int)get(level, x, y-1, z) + (int)get(level, x, y+1, z)
               + (int)get(level, x, y, z-1) + (int)get(level, x, y, z+1);
  if (level > 0)
  {
    numFaces += 3*(int)get(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ);
    numFaces += (int)get(level-1, x>>1, (y>>1)-dirY, (z>>1)-dirZ);
    numFaces += (int)get(level-1, (x>>1)-dirX, y>>1, (z>>1)-dirZ);
    numFaces += (int)get(level-1, (x>>1)-dirX, (y>>1)-dirY, z>>1);
  }

  int numEdges = (int)get(level, x-1, y-1, z) + (int)get(level, x-1, y+1, z) + (int)get(level, x+1, y+1, z) + (int)get(level, x+1, y-1, z)
               + (int)get(level, x-1, y, z-1) + (int)get(level, x-1, y, z+1) + (int)get(level, x+1, y, z+1) + (int)get(level, x+1, y, z-1)
               + (int)get(level, x, y-1, z-1) + (int)get(level, x, y-1, z+1) + (int)get(level, x, y+1, z+1) + (int)get(level, x, y+1, z-1);
  if (level > 0)
  {
    numEdges += 3*(int)get(level-1, (x>>1)-dirX, (y>>1)-dirY, (z>>1)-dirZ);
    numEdges += 2*(int)get(level-1, x>>1, (y>>1)-dirY, (z>>1)-dirZ);
    numEdges += 2*(int)get(level-1, (x>>1)-dirX, y>>1, (z>>1)-dirZ);
    numEdges += 2*(int)get(level-1, (x>>1)-dirX, (y>>1)-dirY, z>>1);
    numEdges += (int)get(level-1, (x>>1)-dirX, y>>1, z>>1);
    numEdges += (int)get(level-1, x>>1, (y>>1)-dirY, z>>1);
    numEdges += (int)get(level-1, x>>1, y>>1, (z>>1)-dirZ);
  }

  if (numEdges < 9)
    numEdges = 9;
  else if (numEdges > 15)
    numEdges = 15;
  if (numFaces < 4)
    numFaces = 4;
  else if (numFaces > 8)
    numFaces = 8;
  if (numCentres < 3)
    numCentres = 3;
  else if (numCentres > 7)
    numCentres = 7;

  numEdges-=9; // 0 to 6
  numFaces-=4; // 0 to 4
  numCentres-=3; // 0 to 4

  int total = (numEdges*5 + numFaces)*5 + numCentres; // max = 174
  if (total < 87)
    return genome[total]; // genome size 87
  else if (total > 87)
    return !genome[174 - total];
  else 
    return self;
}

