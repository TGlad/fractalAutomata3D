#pragma once
#include "basics.h"
#include <windows.h>
/*
class Thread
{
  public:
    Thread();
    int start(void* arg);
  protected:
    int run(void* arg);
    static void* entryPoint(void*);
    virtual void setup() = 0;
    virtual void execute(void*) = 0;
    void* arg;
  private:
    unsigned long threadId;
};

Thread::Thread() {}

int Thread::start(void* arg)
{
  this->arg = arg; // store user data
  int code = CreateThread(NULL, 0, Thread::entryPoint, this, 0, &threadId);
  return code;
}

int Thread::run(void* arg)
{
  setup();
  execute(arg);
}

/*static 
void * Thread::entryPoint(void* pthis)
{
  Thread * pt = (Thread*)pthis;
  pthis->run(arg);
}


*/

class Thread 
{ 
  static unsigned long WINAPI ThreadFunc(LPVOID pv) 
  { 
    try 
    { 
      Thread* thred = reinterpret_cast<Thread *>(pv);
      thred->run(); 
      thred->isRunning = false;
    } 
    catch(...) 
    { 
    } 
    return 0; 
  } 
  
public: 
  typedef unsigned long threadid; 
 
  Thread() 
  { 
    isRunning = false;
  } 
  virtual ~Thread() 
  { 
  } 
  static threadid getThreadID() 
  { 
    return ::GetCurrentThreadId(); 
  } 

  HANDLE handle; 
  
  threadid start() 
  { 
    isRunning = true;
    threadid id; 
    handle = ::CreateThread(NULL, 0, ThreadFunc, this, 0, &id); 
    return id; 
  } 
  bool isRunning;

protected:
  virtual void run()=0; 
}; 