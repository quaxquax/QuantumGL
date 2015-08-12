/*!
	\file		WinThreads.cpp
	\author		Roman Weinberger
	\date		15-11-2002
	
	simple win32 thread class...
*/
#include "WinThreads.h"

static DWORD WINAPI fakeThreadProc(WinThread *pThis)
{
  return pThis->worker();
}

void WinThread::wait()
{
    if( hThread ) {
        suspended = true;
        SuspendThread(hThread);
    }
};

void WinThread::resume()
{
    if( hThread ) {
        suspended = false;
        ResumeThread(hThread);
    }
}


WinThread::WinThread()
{
    threadID = 0;
    hThread  = NULL;
    running  = false;
};

WinThread::~WinThread()
{
    finish();
    CloseHandle(hThread);
};

void WinThread::start()
{
    if( hThread ) finish();
    suspended = false;
    hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) &fakeThreadProc,
    						this, 0, &threadID);
    running = true;
};

void WinThread::finish()
{
    if( !hThread || !running ) return;

    if( suspended ) resume();
    if( WaitForSingleObject(hThread, 1000) == WAIT_TIMEOUT ) TerminateThread(hThread, 1);
        
    CloseHandle(hThread);
    hThread = NULL;
    running = false;
};

bool WinThread::isRunning()
{
    return running;
}


Guard::Guard()
{
    hMutex = CreateMutex(NULL, false, NULL);
}


Guard::~Guard()
{
    if( hMutex != NULL )
    {
        CloseHandle(hMutex);
        hMutex = NULL;
    }
}


void Guard::on() const
{
    WaitForSingleObject(hMutex, INFINITE); 
}


void Guard::off() const
{
    ReleaseMutex(hMutex);
}
  
