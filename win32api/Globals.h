/*!
	\file		Globals.h
	\author		Roman Weinberger
	\date		15-12-2002
	
	global var's / fun's
*/
#ifndef GLOBALS_INC
#define GLOBALS_INC

#include <windows.h>
#include <GL/gl.h>
#include "WorkerThread.h"
#include "../QuantumDescription.h"
#include "../RealVariable.h"
#include "ThreadQueue.h"

#include <cstdio>

extern HINSTANCE	hInstance;
extern HWND         hwndMain;

extern HWND         hwndGl;
extern HWND         hwndDialog;

extern WorkerThread Worker;     // opengl thread
extern qThreadQueue theQueue;   // worker - message queue

extern bool ExportImage(const char *name);  // exports ogl backbuffer
extern void checkMessages();                // keeps windows up to date...

extern bool bSingleFrame;

// parser
int yyparse(void);
extern FILE *yyin;


#endif
