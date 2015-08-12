/*!
	\file		WinThreads.h
	\author		Roman Weinberger
	\date		15-11-2002
	
	simple win32 thread class...
*/
#include <windows.h>

#ifndef WINTHREADS_INC
#define WINTHREADS_INC

class WinThread
{
protected:
  HANDLE  hThread;
  DWORD   threadID;
  volatile bool    running;
  volatile bool    suspended;
public:
  WinThread();
  virtual ~WinThread();
  void start();
  void wait();
  void resume();
  void finish();
  bool isRunning();
  virtual DWORD worker()=0;
};

// simple mutex class
class Guard
{
  HANDLE hMutex;

public:
    Guard();
    virtual ~Guard();
    void on() const;
    void off() const;
};


#endif
