#pragma once
#include "basics.h"
#include "Evolver.h"

class View 
{
public:
  View(int width, int height);
  int width;
  int height;
  class Screen* screen; 
  void recordToScreen(Screen* screen);
  void update();
  void setMaster(int m);
  void resetFromHead(int type, bool isDynamic);
  void load();
  void save();
  void setToLetter(char letter);
  void endThreads();
  
  Evolver* evolvers[7];
  Evolver* bigEvolver; // is allocated and deallocated on the fly
};