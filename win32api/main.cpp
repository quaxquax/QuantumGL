/*!
	\file		main.cpp
	\author		Roman Weinberger
	\date		15-12-2002
	
	main...
*/

/** 
 * backend changes: 
 *      * quantum.tab.cp: #include quantum.tab.c
 *      * lex.yy.cp: #include lex.yy.c
 *      * QuantumDescription - added #ifndef -  #define ... #endif 
 */
 
#define SW_HIDE             0
#define SW_SHOW             5
#define SW_RESTORE          9

#include <windows.h>
#include <COMMCTRL.h>
#ifdef __APPLE__
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif
#include <GL/glu.h>
#include "qMenu.h"
#include "qDialog.h"
#include "WinThreads.h"
#include "PngExport.h"
#include <cstdio>
#include <string.h>
#include "globals.h"
#include "../QuantumDescription.h"
#include "../QuantumConfig.h"
#include "../FieldFunctionUser.h"
#include "../StandardFunctions.h"
#include "../ImageExporter.h"
#include "qDialog.h"
#include "FileSelector.h"

// opengl window class...
#define WINDOW_CLASS_NAME "OpenGL-Window"

// devcpp does not know...
#define GET_X_LPARAM(x) LOWORD(x)
#define GET_Y_LPARAM(x) HIWORD(x)

// callbacks
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GlChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MultiFrameWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// backend interface
void SetProgressMessage(string msg);
void StartDeterminateProgress();
void EndDeterminateProgress();
void ResizeDisplayTo(int x, int y);
void Progress(double x);
void PostRedisplay() { }

// app mode
bool bSingleFrame=false;

// globals
const char	szAppName[] = "QuantumGL";
HINSTANCE	hInstance;
HWND		hwndMain;
HWND		hwndDialog;   // dialog
HWND        hwndGl;       // gl window

// stuff for second window...
const int			nSplitterSize = 200;
const int           nSplitterHeight = 400;

volatile int currFrames=0;

// currentely selected variable
std::string *currSelectionName= new std::string("");

// (de)activates a control
inline void activateControl(DWORD id, BOOL enable=TRUE)
{
    if( enable ) 
        ShowWindow(GetDlgItem(hwndDialog, id), SW_SHOW);
    else
        ShowWindow(GetDlgItem(hwndDialog, id), SW_HIDE);
}

/*
void fileOpened()
{

}
*/

// query registry for current setting
int getFrameSetting()
{
    HKEY hk;
    DWORD res;
    RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\QuantumGL", 0, "Frames",
                   0, KEY_ALL_ACCESS, NULL, &hk, &res); 
    if( res != REG_CREATED_NEW_KEY ) 
    {
        DWORD d=0, s=sizeof(DWORD);
        if( ERROR_SUCCESS != RegQueryValueEx(hk, "setting", NULL, 
                                             NULL, (LPBYTE)&d, &s) ) return -1;
        return d;
    }
    else
    {
        return -1;
    }
}

// set registry entry..
void setFrameSetting(bool multi=false)
{
    HKEY hk;
    DWORD res;
    DWORD set = multi ? 1 : 0;
    RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\QuantumGL", 0, "Frames",
                   0, KEY_ALL_ACCESS, NULL, &hk, &res); 
    RegSetValueEx(hk, "setting", 0, REG_DWORD, (LPBYTE)&set, sizeof(set));
    RegCloseKey(hk);
}


// called when var-selection changed.. ('on' missing)
void varSelected(const std::string &name)
{
    delete currSelectionName;
    currSelectionName = new std::string(name); 
    char buffer[256];
    sprintf(buffer, "%f", Worker.getVarPos(name));
    SetDlgItemText(hwndDialog, IDC_VARIABLE_EDIT, buffer);
    if( Worker.getVarMin(name) == -INFINITY || Worker.getVarMax(name) == INFINITY ) 
    {
        activateControl(IDC_VARIABLE_SLIDER, FALSE);
    }
    else
    { 
        activateControl(IDC_VARIABLE_SLIDER);
        SendDlgItemMessage(hwndDialog, IDC_VARIABLE_SLIDER, TBM_SETRANGEMIN, 
                        TRUE, int(Worker.getVarMin(name) * 100));
        SendDlgItemMessage(hwndDialog, IDC_VARIABLE_SLIDER, TBM_SETRANGEMAX, 
                        TRUE, int(Worker.getVarMax(name) * 100));
        SendDlgItemMessage(hwndDialog, IDC_VARIABLE_SLIDER, TBM_SETPOS, 
                        TRUE, int(Worker.getVarPos(name) * 100));
    }
};

// called if export back buffer is clicked..
void onExportImage()
{
    FileSelector fs;
    if( 1 != fs.modal(hwndMain, Worker.maxViewport[0], Worker.maxViewport[1]) ) return;
    // send it to the thread  
    qThreadMessage *msg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
    msg->id = TM_EXPORT_BACK;
    strncpy(msg->str_data, fs.name.c_str(), 255);
    msg->str_data[255] = 0;
    msg->rect_data.right = fs.w;
    msg->rect_data.bottom = fs.h;
    theQueue.add(msg);
};

// called when exporting to OBJ format
void onExportOBJ()
{
    char szFile[MAX_PATH] = "";
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwndMain;
    ofn.lpstrFilter = "OBJ Files (*.obj)\0*.obj\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = "obj";
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if(GetSaveFileName(&ofn))
    {
        qThreadMessage *msg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
        msg->id = TM_EXPORT_OBJ;
        msg->str_data = ofn.lpstrFile;
        theQueue.add(msg);
    }
};

// reads opengl draw buffer and exports it to a .png
bool ExportImage(const char *name)
{
  SetProgressMessage("Exporting image...");
  StartDeterminateProgress();
  // get gl stuff
  GLint viewport[4];
  glDrawBuffer(GL_BACK);
  Render();
  glGetIntegerv(GL_VIEWPORT, viewport);
  int width = viewport[2];
  int height = viewport[3];
  int colorDepth = 32;
  // could be new...
  DWORD *data = (DWORD *)HeapAlloc(GetProcessHeap(), 0, width*height*4); 
  glFinish();
  // read pixels...
  glReadBuffer(GL_BACK);
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  // export png
  if( !exportToPng(width, height, data, name) ) { MessageBox(hwndMain, "Exporting failed!", "", MB_OK); return false; }
  HeapFree(GetProcessHeap(), 0, data);
  return true;
};

// animation exporter 
void onExportAnimation(bool all=false)
{   
    FileSelector fs;
    if( 1 != fs.modal(hwndMain, Worker.maxViewport[0], Worker.maxViewport[1], false) ) return;
    // send it to the thread  
    qThreadMessage *msg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
    msg->id = all ? TM_EXPORT_ALL_ANIMATION : TM_EXPORT_CURR_ANIMATION;
    strncpy(msg->str_data, fs.name.c_str(), 255);
    msg->str_data[255] = 0;
    msg->rect_data.right = fs.w;
    msg->rect_data.bottom = fs.h;
    theQueue.add(msg);
}


// file dialog
void onFileOpen()
{
    // for quick loading...
    if( Worker.bWorking ) Worker.doAbort = true; 
    
    char szFile[260];
    ZeroMemory(szFile, 260);
    // get file and open    
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwndMain;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "QuantumGL Scripts\0*.qgl\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if(!GetOpenFileName(&ofn)) return;
    
    yyin = fopen(ofn.lpstrFile,"rb"); // ###

    qThreadMessage *msg =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
    msg->id = TM_LOAD_FILE;
    theQueue.add(msg);
};

// quick peek on messages queue
void checkMessages()
{
    MSG msg;
    while(PeekMessage(&msg, hwndMain,0,0, PM_REMOVE))
   {
		if( !IsDialogMessage(hwndDialog, &msg) ) {
                  TranslateMessage(&msg);
		          DispatchMessage(&msg);
        }
   }
   while(PeekMessage(&msg, hwndGl,0,0, PM_REMOVE))
   {
		if( !IsDialogMessage(hwndDialog, &msg) ) {
                  TranslateMessage(&msg);
		          DispatchMessage(&msg);
        }
   }  
   while(PeekMessage(&msg, hwndDialog,0,0, PM_REMOVE))
   {
		if( !IsDialogMessage(hwndDialog, &msg) ) {
                  TranslateMessage(&msg);
		          DispatchMessage(&msg);
        }
   }  
};

// (de)activates a menuitem
void enableMenuItem(UINT id, BOOL doit=TRUE)
{
    EnableMenuItem(GetSubMenu(GetMenu(hwndMain), 1), id, MF_BYCOMMAND | 
                    (doit ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
}

// (de)activates all animation controls
void enableVidControls(BOOL doit=TRUE)
{
    activateControl(IDC_VIDEO_PAUSE, doit);
    activateControl(IDC_VIDEO_PLAY, doit);
    activateControl(IDC_VIDEO_STOP, doit);
    activateControl(IDC_VIDEO_SLIDER, doit);
    activateControl(IDC_VIDEO_FRAME, doit);
    activateControl(IDC_VIDEO_NUM, doit);
    activateControl(IDC_NEXT_ANI, doit);
    activateControl(IDC_PREV_ANI, doit);
    activateControl(IDC_ANI, doit);
    activateControl(IDC_FRM, doit);
    enableMenuItem(IDM_EXPORT_CURRENT_ANIMATION, doit);
    enableMenuItem(IDM_EXPORT_ALL_ANIMATIONS, doit);
}

// enables menuitem on file-load
void fileLoaded()
{
    enableMenuItem(IDM_EXPORT_BACKBUFFER, TRUE);
};

// sets the animation controls
void setAnimationInfo(int currAni, int numAni, int currFrm, int numFrm)
{
    char buffer[256];
    sprintf(buffer, "%d of %d", numAni > 0 ? currAni+1 : 0, numAni);
    SetDlgItemText(hwndDialog, IDC_VIDEO_NUM, buffer);
    sprintf(buffer, "%d of %d", numFrm > 0 ? currFrm+1 : 0, numFrm);
    SetDlgItemText(hwndDialog, IDC_VIDEO_FRAME, buffer);
    
    SendDlgItemMessage(hwndDialog, IDC_VIDEO_SLIDER, TBM_SETRANGE, TRUE, MAKELONG(0, numFrm-1)); 
    SendDlgItemMessage(hwndDialog, IDC_VIDEO_SLIDER, TBM_SETPOS, TRUE, currFrm); 
    
    currFrames = numFrm;
    
    checkMessages();
    
    if( !numAni || !numFrm ) {
        enableVidControls(FALSE);
    } else {
        enableVidControls();
    }
    checkMessages();
}

// progress message
void SetProgressMessage(string msg)
{
   SetDlgItemText(hwndDialog, IDC_PROGRESS_OUT, msg.c_str());
   checkMessages();
}

void StartDeterminateProgress()
{
    SendDlgItemMessage(hwndDialog, IDC_PROGRESS_BAR, PBM_SETPOS, 0, 0); 
}

// unused
void EndDeterminateProgress() { }

// progress bar get's set to ...
void Progress(double x)
{
    SendDlgItemMessage(hwndDialog, IDC_PROGRESS_BAR, PBM_SETPOS, int(100*x), 0); 
}


// somehow mingw does not recognize WinMain as entry point
int main()
{
    //freopen("log.txt", "wt", stdout);
    return WinMain(GetModuleHandle(NULL), NULL, "", 1);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{	
	InitCommonControls();  
	
	// uncomment for debug output..
	
	// AllocConsole();
	// freopen("CONOUT$", "wb", stdout);
	// cout.sync_with_stdio();
	
	WNDCLASSEX	wc;
	MSG			msg;
	
    // setting window-class
    wc.cbSize			= sizeof(wc);
	wc.style			= 0;
	
#if 0
	// correct handle function...
    int r = getFrameSetting();
    if( r == -1 ) {
        //int res = MessageBox(HWND_DESKTOP, "Do you want to use QuantumGL in multiframe mode?", 
        //                     "QuantumGL", MB_YESNO);
        setFrameSetting(0);
        r = 0;
    }
    if( r == 0 ) {
        wc.lpfnWndProc		= WndProc;
        bSingleFrame = true;   
    } else if( r == 1 ) {
        wc.lpfnWndProc		= MultiFrameWndProc;
        bSingleFrame = false;
    }
 #else
 	wc.lpfnWndProc		= MultiFrameWndProc;
    bSingleFrame = false;
 #endif
 
    wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon (NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_BACKGROUND);;
	wc.lpszMenuName		= "qMenu";
	wc.lpszClassName	= szAppName;
	wc.hIconSm			= LoadIcon (NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);
	hInstance = hInst;
	
	int adjustedWidth, adjustedHeight;
 	{
 		RECT r;
 		SetRect(&r,0,0,512,512);
 		AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,TRUE);
 		adjustedWidth = r.right - r.left;
 		adjustedHeight = r.bottom - r.top;
	}
	
	hwndMain = CreateWindowEx(0, szAppName,	szAppName,
 				WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,		
				CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight,
    			NULL, NULL, hInst, NULL);					
	
    ShowWindow(hwndMain, nShowCmd);

    enableVidControls(FALSE);
    enableMenuItem(IDM_EXPORT_BACKBUFFER, FALSE);
    
	while(GetMessage(&msg, NULL,0,0))
	{
		if( !IsDialogMessage(hwndDialog, &msg) ) {
                  TranslateMessage(&msg);
		          DispatchMessage(&msg);
        }
	}
	return 0;
}

// finishes the thread...
void closeThread()
{
    qThreadMessage *m = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
    m->id = TM_QUIT;
    theQueue.add(m);
    Worker.finish();
};

// client xy-pos
void GetXY(int *x, int *y)
{
    RECT rw, rc;
    GetWindowRect(hwndMain, &rw);
    GetClientRect(hwndMain, &rc);
    
    *x = rw.left + ((rw.right-rw.left) - rc.right);
    *y = rw.top + ((rw.bottom-rw.top) - rc.bottom);
};

// window moving...
void handleMove()
{
    /*if( !bSingleFrame ) return;
    int x,y;
	GetXY(&x, &y);
    RECT rc;
    GetClientRect(hwndMain, &rc);
	MoveWindow(hwndDialog, x+rc.right - nSplitterSize, y, nSplitterSize-6, rc.bottom-6, TRUE);*/
};

// window-pos
void getWindowXY(HWND hwnd, int &x, int &y)
{
    RECT r;
    GetWindowRect(hwnd, &r);
    x = r.left;
    y = r.top;
};

// size-add for menu...
void getWindowXYAdd(HWND hwnd, int &x, int &y)
{
    RECT rw, rc;
    GetWindowRect(hwndMain, &rw);
    GetClientRect(hwndMain, &rc);
    
    x = (rw.right  - rw.left) - (rc.right  - rc.left);
    y = (rw.bottom - rw.top ) - (rc.bottom - rc.top );
}

// only in multiframe mode
void ResizeDisplayTo(int x, int y) 
{
    if( !bSingleFrame ) {
        int xx, yy;
        int xa, ya;
        getWindowXY(hwndMain, xx, yy);
        getWindowXYAdd(hwndMain, xa, ya);
        MoveWindow(hwndMain, xx, yy, x+xa, y+ya, TRUE);
        reshape(x, y);
    }
}

void SizeWindowContents(int nWidth, int nHeight)
{
    // resize gl window
	if( bSingleFrame ) 
	{
        // gl window
        qThreadMessage *msg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
        msg->id = TM_RESIZE_PORT;
        msg->rect_data.top = msg->rect_data.left = 0;
        msg->rect_data.right = nWidth-nSplitterSize;
        msg->rect_data.bottom = nHeight;
        theQueue.add(msg);
        // and dialog
        int x,y;
	    GetXY(&x, &y);
	    int xx = x+nWidth - nSplitterSize;
	    int ww = nSplitterSize - 6;
        MoveWindow(hwndDialog, xx < x ? x : xx, y-4, (ww > nWidth - 6 ? nWidth - 6 : ww)+4, nHeight, TRUE);
    } 
}

void createGlWindow(HWND hwnd)
{
    WNDCLASSEX mWC;
    
    ZeroMemory (&mWC, sizeof (WNDCLASSEX));						
	mWC.cbSize			= sizeof (WNDCLASSEX);					
	mWC.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	mWC.lpfnWndProc		= GlChildWndProc;				
	mWC.hInstance		= hInstance;				
	mWC.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			
	mWC.hCursor			= LoadCursor(NULL, IDC_ARROW);			
	mWC.lpszClassName	= WINDOW_CLASS_NAME;				

   if(RegisterClassEx (&mWC) == 0) {
		MessageBox(HWND_DESKTOP, "Error registering opengl window!", "Error", MB_OK | MB_ICONEXCLAMATION);		
	}
    
    hwndGl  = CreateWindowEx(0, WINDOW_CLASS_NAME,	
						   "", WS_CHILD, 0, 0, 0, 0, hwnd,						
						   0, hInstance, NULL);
	
	
	ShowWindow(hwndGl, SW_NORMAL);
	
    // thread should build dc/rc					   
    qThreadMessage *tmsg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
    tmsg->id = TM_INIT_GL;
    tmsg->hwnd_data = hwndGl;
    theQueue.add(tmsg);
    Worker.start();
}

// cycles animations
void changeAnimation(bool next=true)
{
    if( Worker.playingAnimation ) 
    { 
        qThreadMessage *m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
        m->id = TM_STOP_ANIMATION;
        theQueue.add(m);
   }
   qThreadMessage *m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
   m->id = next ? TM_NEXT_ANIMATION : TM_PREV_ANIMATION;
   theQueue.add(m);
}



//---------------------------------------------------------------------
//
//  Single Frame Callbacks
//
//---------------------------------------------------------------------


// main window callback
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    qThreadMessage *tmsg;
	switch(msg)
	{
	case WM_COMMAND:
	     switch(LOWORD(wParam))
	     {
         case IDM_FILE_OPEN:
                  onFileOpen();
         break;
         case IDM_EXPORT_BACKBUFFER:
                  onExportImage();
         break;
         case IDM_EXPORT_OBJ:
                  onExportOBJ();
         break;
         case IDM_FILE_EXIT:
                  //printf("Exit menu-item selected...\n");
                  PostMessage(hwnd, WM_CLOSE, 0, 0);
         break;
         case IDM_EXPORT_CURRENT_ANIMATION:
                onExportAnimation();
         break;
         case IDM_EXPORT_ALL_ANIMATIONS:
                onExportAnimation(true);
         break;
         case IDM_SETTINGS_MULTI: 
            setFrameSetting(true);
            MessageBox(hwnd, "Settings will be applied after you restart QuantumGL", "Info", MB_OK);
         break;
         case IDM_SETTINGS_SINGLE:
            setFrameSetting(false);
            MessageBox(hwnd, "Settings will be applied after you restart QuantumGL", "Info", MB_OK);
         break;
	     };
	break;
	case WM_CREATE:
         hwndDialog = CreateDialog(hInstance, "qDialog", hwnd, DlgProc);
         activateControl(IDC_VARIABLE_SLIDER, FALSE);
         enableVidControls(FALSE);
         createGlWindow(hwnd);
         return 0;
	case WM_SIZE:
	    //printf("WM_SIZE\n");
		SizeWindowContents(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_CLOSE:
	    //printf("WM_CLOSE received - closing thread...\n");
	    fflush(stdout);
	    //Worker.doAbort = true;
	    closeThread();
	    //printf("Sending WM_DESTROY...\n");
	    fflush(stdout);
		DestroyWindow(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MOVE:
	     handleMove(); 
	   return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


// tools-dialog - ugly win32
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    char tmp[300];
    ZeroMemory(tmp, 300);
    HWND hTmp;
    int nTmp;
    float x;
    qThreadMessage *m;
    switch(msg)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hwnd, IDC_PROGRESS_OUT, "idle");//SendDlgItemMessage(hwnd, EM_SETTEXTEX, IDC_PROGRESS_OUT, 0, (LPARAM)str);
        SendDlgItemMessage(hwnd, IDC_VIDEO_PLAY, BM_SETIMAGE, IMAGE_BITMAP,
            (LPARAM)LoadImage(hInstance, MAKEINTRESOURCE(IDC_BITMAP_PLAY), IMAGE_BITMAP, 10, 10, 0));
        SendDlgItemMessage(hwnd, IDC_VIDEO_PAUSE, BM_SETIMAGE,IMAGE_BITMAP,
            (LPARAM)LoadImage(hInstance, MAKEINTRESOURCE(IDC_BITMAP_PAUSE), IMAGE_BITMAP, 10, 10, 0));
        SendDlgItemMessage(hwnd, IDC_VIDEO_STOP, BM_SETIMAGE,IMAGE_BITMAP,
            (LPARAM)LoadImage(hInstance, MAKEINTRESOURCE(IDC_BITMAP_STOP), IMAGE_BITMAP, 10, 10, 0));
    break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) 
            { 
                case IDC_VARIABLE_LIST: 
                    switch (HIWORD(wParam)) 
                    { 
                        case LBN_SELCHANGE: 
                           nTmp = SendDlgItemMessage(hwnd, IDC_VARIABLE_LIST, 
                                                      LB_GETCURSEL, 0, 0); 
                           nTmp = SendDlgItemMessage(hwnd, IDC_VARIABLE_LIST, 
                                                      LB_GETTEXT, nTmp, (LPARAM)tmp);
                           if( nTmp == LB_ERR ) return TRUE;
                           tmp[nTmp] = 0;               
                           varSelected(tmp);
                        return TRUE; 
                   } 
                break;
                case IDC_VIDEO_PLAY:
                    m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
                    m->id = TM_PLAY_ANIMATION;
                    theQueue.add(m);
                break;
                case IDC_VIDEO_PAUSE:
                   if( Worker.playingAnimation ) 
                   { 
                        m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
                        m->id = TM_PAUSE_ANIMATION;
                        theQueue.add(m);
                   }
                break;
                case IDC_VIDEO_STOP:
                   m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
                   m->id = TM_STOP_ANIMATION;
                   theQueue.add(m);
                break;
                case IDC_NEXT_ANI:
                   changeAnimation();
                break;
                case IDC_PREV_ANI:
                   changeAnimation(false);
                break;
            }
            
            // hacky
            if( wParam == IDOK ) 
            {  
                GetDlgItemText(hwnd, IDC_VARIABLE_EDIT, tmp, 300);
                x = atof(tmp);
                m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
                m->id = TM_CHANGE_VAR;
                ZeroMemory(m->str_data, 256); 
                memcpy(m->str_data, currSelectionName->c_str(), strlen(currSelectionName->c_str()));
                m->float_data = x;
                theQueue.add(m);
                return TRUE;
             }
    break;
    case WM_HSCROLL:
        if( (HWND)lParam == GetDlgItem(hwnd, IDC_VARIABLE_SLIDER) ) 
        {
            if( !(LOWORD(wParam) == SB_THUMBPOSITION || 
                LOWORD(wParam) == SB_PAGELEFT || 
                LOWORD(wParam) == SB_PAGERIGHT) ) return TRUE;
            nTmp = SendDlgItemMessage(hwnd, IDC_VARIABLE_SLIDER, TBM_GETPOS, 0, 0);
            x = nTmp / 100.0f;
            sprintf(tmp, "%f", x);
            SetDlgItemText(hwndDialog, IDC_VARIABLE_EDIT, tmp);
            m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
            m->id = TM_CHANGE_VAR;
            ZeroMemory(m->str_data, 256); 
            memcpy(m->str_data, currSelectionName->c_str(), strlen(currSelectionName->c_str()));
            m->float_data = x;
            theQueue.add(m);
        } 
        else if( (HWND)lParam == GetDlgItem(hwnd, IDC_VIDEO_SLIDER) ) 
        {
            nTmp = SendDlgItemMessage(hwnd, IDC_VIDEO_SLIDER, TBM_GETPOS, 0, 0);
            
            if( !(LOWORD(wParam) == SB_THUMBPOSITION || 
                  LOWORD(wParam) == SB_PAGELEFT || 
                  LOWORD(wParam) == SB_PAGERIGHT) ) 
            {
                sprintf(tmp, "%d of %d", currFrames > 0 ? nTmp+1 : 0, currFrames);
                SetDlgItemText(hwnd, IDC_VIDEO_FRAME, tmp);
                return TRUE;
            }
            
            m =(qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
            m->id = TM_SET_ANI_FRAME;
            m->int_data = nTmp;
            theQueue.add(m);
        }
    break;
    };
    return 0;
};


// gl-window
LRESULT CALLBACK GlChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    qThreadMessage *tmsg;
    static bool mdown=false;
    PAINTSTRUCT ps;
    switch(msg) 
    {
    case WM_PAINT:
        if( Worker.bFileLoaded && !Worker.bWorking ) {
            ValidateRect(hwnd, NULL);
            tmsg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
            tmsg->id = TM_DRAW;
            theQueue.add(tmsg);
        } 
    break;
    case WM_ERASEBKGND:
        if( !Worker.bFileLoaded ) {
            break;
        }
        else return 0;
    case WM_LBUTTONDOWN:
        if( Worker.bFileLoaded && !Worker.bWorking ) { 
                mdown = true;
                mouse(kMBLeft, kMBDown, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_MBUTTONDOWN:
        if( Worker.bFileLoaded && !Worker.bWorking ){
                 mdown = true;
                mouse(kMBMiddle, kMBDown, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_RBUTTONDOWN:
        if( Worker.bFileLoaded && !Worker.bWorking ) {
                 mdown = true;
                mouse(kMBRight, kMBDown,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_LBUTTONUP:
        mdown = false;
        if( Worker.bFileLoaded && !Worker.bWorking ) {
                mouse(kMBLeft, kMBUp, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_MBUTTONUP:
        mdown = false;
        if( Worker.bFileLoaded && !Worker.bWorking ) { 
                mouse(kMBMiddle, kMBUp, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_RBUTTONUP:
        mdown = false;
        if( Worker.bFileLoaded && !Worker.bWorking ) {
                 mdown = false;
                mouse(kMBRight, kMBUp, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_MOUSEMOVE:
        if( Worker.bFileLoaded && !Worker.bWorking && mdown ) {          
                motion(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    };
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


//---------------------------------------------------------------------
//
//  Multi Frame Callbacks
//
//---------------------------------------------------------------------
// main window callback
LRESULT CALLBACK MultiFrameWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    qThreadMessage *tmsg;
    static bool mdown=false;
    int x,y;
    PAINTSTRUCT ps;
    RECT r;
    
	switch(msg)
	{
	case WM_COMMAND:
	     switch(LOWORD(wParam))
	     {
         case IDM_FILE_OPEN:
                  onFileOpen();
         break;
         case IDM_EXPORT_BACKBUFFER:
                  onExportImage();
         break;
         case IDM_EXPORT_OBJ:
                  onExportOBJ();
         break;
         case IDM_FILE_EXIT:
                  PostMessage(hwnd, WM_CLOSE, 0, 0);
         break;
         case IDM_EXPORT_CURRENT_ANIMATION:
                onExportAnimation();
         break;
         case IDM_EXPORT_ALL_ANIMATIONS:
                onExportAnimation(true);
         break;
         case IDM_SETTINGS_MULTI: 
            setFrameSetting(true);
            MessageBox(hwnd, "Settings will be applied after you restart QuantumGL", "Info", MB_OK);
         break;
         case IDM_SETTINGS_SINGLE:
            setFrameSetting(false);
            MessageBox(hwnd, "Settings will be applied after you restart QuantumGL", "Info", MB_OK);
         break;
	     };
	break;
	case WM_CREATE:
         hwndDialog = CreateDialog(hInstance, "qDialog", hwnd, DlgProc);
         SetWindowLong(hwndDialog, GWL_STYLE, WS_POPUP|WS_CAPTION|WS_BORDER|WS_VISIBLE);
         SetWindowText(hwndDialog, "QuantumGL Control");
         GetWindowRect(hwnd,&r);
         MoveWindow(hwndDialog, r.right + 2, r.top, nSplitterSize, nSplitterHeight, TRUE);
         activateControl(IDC_VARIABLE_SLIDER, FALSE);
         enableVidControls(FALSE);
         hwndMain = hwndGl = hwnd;
         tmsg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
         tmsg->id = TM_INIT_GL;
         tmsg->hwnd_data = hwndGl;
         theQueue.add(tmsg);
         Worker.start();
         return 0;
	case WM_SIZE:
	    tmsg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
        tmsg->id = TM_RESIZE_PORT;
        tmsg->rect_data.left = tmsg->rect_data.top = 0;
        tmsg->rect_data.right = LOWORD(lParam);
        tmsg->rect_data.bottom = HIWORD(lParam);
        theQueue.add(tmsg);
        return 0;		
    case WM_CLOSE:
	    Worker.doAbort = true;
	    closeThread();
		DestroyWindow(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
    case WM_PAINT:
        if( Worker.bFileLoaded && !Worker.bWorking ) {
            ValidateRect(hwnd, NULL);
            tmsg = (qThreadMessage*)HeapAlloc(GetProcessHeap(), 0, sizeof(qThreadMessage));
            tmsg->id = TM_DRAW;
            theQueue.add(tmsg);
            return 1;
        } else {
            break;
        }
    case WM_ERASEBKGND:
        if( !Worker.bFileLoaded ) 
            break;
        else {
            //PostMessage(hwnd, WM_PAINT, 0, 0);
            return 0;
        }
    case WM_LBUTTONDOWN:
        if( Worker.bFileLoaded && !Worker.bWorking ) { 
                mdown = true;
                mouse(kMBLeft, kMBDown, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_MBUTTONDOWN:
        if( Worker.bFileLoaded && !Worker.bWorking ){
                 mdown = true;
                mouse(kMBMiddle, kMBDown, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_RBUTTONDOWN:
        if( Worker.bFileLoaded && !Worker.bWorking ) {
                 mdown = true;
                mouse(kMBRight, kMBDown,GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_LBUTTONUP:
        mdown = false;
        if( Worker.bFileLoaded && !Worker.bWorking ) {
                mouse(kMBLeft, kMBUp, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_MBUTTONUP:
        mdown = false;
        if( Worker.bFileLoaded && !Worker.bWorking ) { 
                mouse(kMBMiddle, kMBUp, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_RBUTTONUP:
        mdown = false;
        if( Worker.bFileLoaded && !Worker.bWorking ) {
                 mdown = false;
                mouse(kMBRight, kMBUp, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
    case WM_MOUSEMOVE:
        if( Worker.bFileLoaded && !Worker.bWorking && mdown ) {          
                motion(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SendMessage(hwnd, WM_PAINT, 0,0);
        }
    return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


