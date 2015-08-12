/*!
	\file		ThreadQueue.h
	\author		Roman Weinberger
	\date		15-11-2002
	
	thread message stuff
*/
#include <vector>
#include <list>
#include <string>
#include <windows.h>
#include "WinThreads.h"

// thread message - id's 
#define TM_IDLE             0   // unused
#define TM_INIT_GL          1   // hwnd_data must be filled
#define TM_LOAD_FILE        2   // str_data must be filled
#define TM_DRAW             3   
#define TM_CHANGE_VAR       4   // str_data and float_data must be filled
#define TM_EXPORT_BACK      5   // str_data must be filled
#define TM_RESIZE_PORT      6   // rect_data must be filled

#define TM_PLAY_ANIMATION   8    
#define TM_PAUSE_ANIMATION  9
#define TM_STOP_ANIMATION   10  // sets back to currFrame = 0 

#define TM_SET_ANI_FRAME    11  // int_data must be filled
#define TM_NEXT_ANIMATION   12  
#define TM_PREV_ANIMATION   13

#define TM_EXPORT_CURR_ANIMATION    14 // str_data must be filled
#define TM_EXPORT_ALL_ANIMATION     15 // ---       ""        ---

#define TM_QUIT             666 


#ifndef THREADQUEUE_INC
#define THREADQUEUE_INC

// message sent to thread - inefficient, but works...
class qThreadMessage
{
public:
    DWORD id;
    char  str_data[256];
    float float_data;
    int   int_data;
    HWND  hwnd_data;
    bool important;
    RECT rect_data;
    
    qThreadMessage(DWORD i=0, const char s[]="", float f=0.0f, HWND h=NULL, bool b=false) 
    :
    id(i),  
    float_data(f), important(b), hwnd_data(h)
    {
        memcpy(str_data, s, strlen(s));
    }
};

// queue that stores thread messages
// - todo: add alloc/dealloc to ensure usage of HeapAlloc()
// - todo: singleton?
class qThreadQueue
{
    std::list<qThreadMessage *> messages;
public:
    qThreadQueue() { }
    
    void add(qThreadMessage *q);

    qThreadMessage *guardAccess(qThreadMessage *q = NULL, bool skip_redr=false, bool push_back=false);

    qThreadMessage *skipResize() { return guardAccess(NULL, true); }

    void addFront(qThreadMessage *q) { throw("Not working"); };
        
    qThreadMessage *getNext() { return guardAccess(); }
    
    qThreadMessage *peekMsg() {
        Guard g;
        g.on();
        if( messages.empty() ) return NULL;
        qThreadMessage *m = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
        memcpy(m, *messages.begin(), sizeof(qThreadMessage)); 
        g.off(); 
        return m;
    }
    
    bool empty() { return messages.empty(); }
    
    ~qThreadQueue() { 
        while( !messages.empty() ) { 
                HeapFree(GetProcessHeap(), 0, *messages.begin()); 
                messages.pop_front(); 
        }
    }
};

extern qThreadQueue theQueue;

#endif
