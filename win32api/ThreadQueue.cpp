/*!
	\file		ThreadQueue.cpp
	\author		Roman Weinberger
	\date		15-11-2002
	
	thread message stuff
*/
#include "ThreadQueue.h"
#include "globals.h"

// adds a new message and then try's to wake up worker..
void qThreadQueue::add(qThreadMessage *q) 
{
        guardAccess(q, false, true);
        Worker.resume();
};


qThreadMessage *qThreadQueue::guardAccess(qThreadMessage *q, 
        bool skip_redr, bool push_back)
{
    Guard g;
    g.on(); 
    qThreadMessage *m=NULL;
    if( q ) 
    {
        if( push_back ) messages.push_back(q);
        else messages.push_front(q); 
    }
    else if( skip_redr )
    {
        qThreadMessage *lr=NULL;
            
        while(1)
        {
                if( messages.empty() ) return lr;
                m = *messages.begin();
                if( m->id == TM_RESIZE_PORT ) {
                        if( lr != NULL ) {
                            HeapFree(GetProcessHeap(), 0, lr);
                        }
                        lr = m;
                        messages.pop_front();
                } else {
                        return lr;
                }
        }
    }
    else
    {
        if( messages.empty() ) return NULL;
        m = *messages.begin(); 
        messages.pop_front();
    }
    g.off();
    return m;
}
