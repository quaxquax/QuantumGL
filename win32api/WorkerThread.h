/*!
	\file		WorkerThread.h
	\author		Roman Weinberger
	\date		15-11-2002
	
	grunt work...
*/
#include "ThreadQueue.h"

#ifndef WORKERTHREAD_INC
#define WORKERTHREAD_INC

#include <GL/gl.h>
#include <GL/glu.h>
#include "qMenu.h"
#include "qDialog.h"
#include "WinThreads.h"

#include "PngExport.h"

#include <cstdio>
#include <string.h>

#include "../QuantumProgress.h"

#include "../QuantumDescription.h"
#include "../QuantumFrontend.h"

#include "../QuantumConfig.h"

#include "../Animator.h"
#include "../VisualObject.h"
#include "../BSPTree.h"
#include "../AutoreleasePool.h"
#include "../FieldFunctionUser.h"
#include "../StandardFunctions.h"
#include "../CubeInfo.h"
#include "../ImageExporter.h"


/**
 * USER FUNCTION:
 */ 

extern void setAnimationInfo(int currAni, int numAni, int currFrm, int numFrm);


class WorkerThread : public WinThread
{
public:
    
    int     maxViewport[2];
    
    volatile bool    bFileLoaded;
    volatile bool    bWorking;
    volatile bool    bAbortLocked;
    volatile bool    doAbort;
    volatile bool    expAnimation;
    volatile bool    doPauseAnimation;
    volatile bool    playingAnimation;
    
    // 0'oing 
    WorkerThread() { 
        mdc = dc = NULL; 
        mrc = rc = NULL; 
        hBmp = NULL;
        playingAnimation = 
        doPauseAnimation = 
        bFileLoaded = 
        bWorking = 
        bAbortLocked = 
        doAbort = false; 
        currFrame = 0;
        currAnimation = 0;
    }
    // main worker function    
    DWORD worker();

    void swapBuffers();
    
    
    
    float getVarMin(const std::string &name);    
    float getVarMax(const std::string &name);
    float getVarPos(const std::string &name);

    int   getNumAnimations();
    int   getNumAnimationFrames(int ani);
    
    
protected:
    HDC     dc;
    HGLRC   rc;

    volatile int currFrame;
    volatile int currAnimation;
    
    HDC     mdc;
    HGLRC   mrc;
    HBITMAP hBmp;
    int     vp[4];
    int exp_w, exp_h;
protected:
    // initializes opengl
    bool initGl(HWND hwnd);
    void closeGl();
    // loads a file
    bool load(const std::string &fname);
    
    // creates a new dc and exports the image
    bool exportGlBitmap(char *name);
    
    // creates the memdc / memrc
    bool initGlExport(int w, int h);
    void closeGlExport();
    
    void exportCurrAnimation(qThreadMessage *m);
    void exportAnimations(qThreadMessage *m);
};


#endif
