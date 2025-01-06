/*!
	\file		WorkerThread.cpp
	\author		Roman Weinberger
	\date		15-11-2002
	
	grunt work...
*/
#include "WorkerThread.h"
#include "globals.h"

// global stuff...
qThreadQueue theQueue;  
WorkerThread Worker;

// extern functions
void SetProgressMessage(string msg);
void StartDeterminateProgress();
void fileLoaded();

void CheckAbort() throw(AbortException)
{
    if( !Worker.bAbortLocked && Worker.doAbort ) throw AbortException();
}

void SwapBuffers() 
{ 
    Worker.swapBuffers();
}

void LockGL() {}
void UnlockGL() {}

extern void SetProgressMessage(std::string s);

// ... (rest of the code remains the same)

DWORD WorkerThread::worker() 
{    
    qThreadMessage *m=NULL;
    while(1) 
    {
        if( (m = theQueue.getNext()) != NULL )  
        {
            if( m->id == TM_QUIT ) 
            {
                closeGl();
                running = false;
                HeapFree(GetProcessHeap(), 0, m);
                printf("TM_QUIT received...\n");
                return 0;
            } 
            else if( m->id == TM_INIT_GL ) 
            {
                bAbortLocked = bWorking = true;
                if( !this->initGl(m->hwnd_data) ) exit(666);                
                bAbortLocked = bWorking = false;
            } 
            else if( m->id == TM_LOAD_FILE ) 
            {
                bAbortLocked = bWorking = true;
                load(m->str_data);
                if( bFileLoaded ) display();
                fileLoaded();
                bAbortLocked = bWorking = false;
            } 
            else if( m->id == TM_DRAW ) 
            {
                if( bFileLoaded ) 
                {
                    if( m->important ) bAbortLocked = true;
                    bWorking = true;
                    display();
                    bAbortLocked = bWorking = false;
                }
            } 
            else if( m->id == TM_EXPORT_BACK ) 
            {
                if( bFileLoaded ) 
                {
                    bAbortLocked = bWorking = true;
                    if( initGlExport(m->rect_data.right, m->rect_data.bottom) ) 
                    { 
                        exportGlBitmap(m->str_data);
                        closeGlExport();
                    }
                    bAbortLocked = bWorking = false;
                }
            }
            else if( m->id == TM_EXPORT_OBJ )
            {
                if (currentObject && dynamic_cast<IsoSurface*>(currentObject))
                {
                    IsoSurface* surface = dynamic_cast<IsoSurface*>(currentObject);
                    OBJExporter exporter;
                    exporter.ExportToOBJ(*surface, m->str_data);
                }
                HeapFree(GetProcessHeap(), 0, m);
            }
            else if( m->id == TM_CHANGE_VAR ) 
            {
                if( bFileLoaded ) 
                {
                    RealVariable *cVar =  definedRealVariables[std::string(m->str_data)];
                    if( cVar ) 
                    {
                        bWorking = true;
                        cVar->ChangeTo(m->float_data);
                        if( m->important ) bAbortLocked = true;
                        display();
                        bAbortLocked = bWorking = false;
                    }
                }
            }
            else if( m->id == TM_RESIZE_PORT ) 
            {
                qThreadMessage *tmp = theQueue.skipResize();
                if( tmp ) {
                    HeapFree(GetProcessHeap(), 0, m);
                    m = tmp;
                }    
                if( bSingleFrame ) 
                                MoveWindow(hwndGl, 0, 0, m->rect_data.right, m->rect_data.bottom, FALSE);　　 　 　 　
                reshape(m->rect_data.right, m->rect_data.bottom);
                PostMessage(hwndGl, WM_PAINT, 0, 0);
            }   
            else if( m->id == TM_PLAY_ANIMATION ) 
            {
                if( bFileLoaded ) 
                {
                    bAbortLocked = bWorking = playingAnimation = true;
                    theAnimationsList::iterator it;
                    bool getout = false; // insted of goto ;)
                    it = theAnimations.begin();
                    for(int xx=0; xx<currAnimation; xx++) it++; // mingw32 does'nt like it+=x;
                    int frames = (*it)->GetFrameCount();
                    for(int j=currFrame; j<frames; j++) {
                        (*it)->SetFrame(j);
                        setAnimationInfo(currAnimation, theAnimations.size(), currFrame, frames);
                        display();
                        currFrame++;
                        // check for messages - i.e. pause / stop - all others are ignored
                        while( (m = theQueue.getNext()) != NULL ) { 
                            if( m->id == TM_PAUSE_ANIMATION ) { 
                                getout = true;
                                break;
                            } else if( m->id == TM_STOP_ANIMATION ) {
                                getout = true;
                                currFrame = 0;
                                
                                (*it)->SetFrame(0);
                                display();
                                break;
                            } else if( m->id == TM_RESIZE_PORT ) {
                                glViewport(m->rect_data.left, m->rect_data.top, 
                                m->rect_data.right, m->rect_data.bottom);
                            }
                        }   
                        if( getout ) break;
                    }
      
                    bAbortLocked = bWorking = playingAnimation = false; 
                }
            }    
            else if( m->id == TM_STOP_ANIMATION ) 
            {
                bAbortLocked = bWorking = playingAnimation = true; 
                theAnimationsList::iterator it = theAnimations.begin();
                for(int xx=0; xx<currAnimation; xx++) it++; // mingw32 does'nt like it+=x;  
                currFrame = 0;
                setAnimationInfo(currAnimation, theAnimations.size(), currFrame, (*it)->GetFrameCount());
                (*it)->SetFrame(0);
                display();
                bAbortLocked = bWorking = playingAnimation = false; 
            }
            else if( m->id == TM_SET_ANI_FRAME ) 
            {
                bAbortLocked = bWorking = playingAnimation = true; 
                theAnimationsList::iterator it = theAnimations.begin();
                for(int xx=0; xx<currAnimation; xx++) it++; // mingw32 does'nt like it+x;  
                currFrame = m->int_data >= (*it)->GetFrameCount() ? (*it)->GetFrameCount()-1 : m->int_data;
                setAnimationInfo(currAnimation, theAnimations.size(), currFrame, (*it)->GetFrameCount());
                (*it)->SetFrame(currFrame);
                display();
                bAbortLocked = bWorking = playingAnimation = false; 
            }
            else if( m->id == TM_NEXT_ANIMATION ) 
            {
                 bAbortLocked = bWorking = playingAnimation = true;
                currAnimation = currAnimation+1 >= theAnimations.size() 
                                    ?
                                currAnimation : currAnimation+1;
                currFrame = 0;
                theAnimationsList::iterator it;
                bool getout = false;
                it = theAnimations.begin();
                for(int xx=0; xx<currAnimation; xx++) it++;
                int frames = (*it)->GetFrameCount();
                (*it)->SetFrame(currFrame);
                setAnimationInfo(currAnimation, theAnimations.size(), currFrame, frames);
                display();
                 bAbortLocked = bWorking = playingAnimation = false;
            }
            else if( m->id == TM_PREV_ANIMATION ) 
            {
                 bAbortLocked = bWorking = playingAnimation = true;
                currAnimation = currAnimation-1 < 0 
                                    ?
                                currAnimation : currAnimation-1;
                currFrame = 0;
                theAnimationsList::iterator it;
                bool getout = false;
                it = theAnimations.begin();
                for(int xx=0; xx<currAnimation; xx++) it++;
                int frames = (*it)->GetFrameCount();
                (*it)->SetFrame(currFrame);
                setAnimationInfo(currAnimation, theAnimations.size(), currFrame, frames);
                display();
                 bAbortLocked = bWorking = playingAnimation = false;          
            }
            else if( m->id == TM_EXPORT_CURR_ANIMATION ) 
            {
                exportCurrAnimation(m);
            }
            else if( m->id == TM_EXPORT_ALL_ANIMATION ) 
            {
                exportAnimations(m);
            }
            
            
            HeapFree(GetProcessHeap(), 0, m);
            
            SetProgressMessage("idle");
        }
        else
        {
            // let the process sleep
            this->wait();
        }
    }
}


float WorkerThread::getVarMin(const std::string &name)
{
    Guard g;
    g.on();
    RealVariable *cVar =  definedRealVariables[name];
    if( cVar == NULL ) return 0.0f;
    return cVar->theRange.first;
}
    
float WorkerThread::getVarMax(const std::string &name)
{
    Guard g;
    g.on();
    RealVariable *cVar =  definedRealVariables[name];
    if( cVar == NULL ) return 0.0f;
    return cVar->theRange.second;
}


float WorkerThread::getVarPos(const std::string &name)
{
    Guard g;
    g.on();
    RealVariable *cVar =  definedRealVariables[name];
    if( cVar == NULL ) return 0.0f;
    return cVar->value;
}


void WorkerThread::swapBuffers()
{
    if( !expAnimation ) if(  !SwapBuffers(dc) ) {}
}
    

// ---------------------- OpenGL ------------------------- //



static PIXELFORMATDESCRIPTOR pfd = 
{
sizeof(PIXELFORMATDESCRIPTOR), 1,                                
PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,                 
PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       
16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 
};

bool WorkerThread::initGl(HWND hwnd)
{
        int PixelFormat;
        dc = GetDC(hwnd);
        PixelFormat = ChoosePixelFormat(dc, &pfd);
        if( PixelFormat == 0 ) { 
                MessageBox(hwnd, "Error getting Gl Pixel Format", "Error", MB_OK); 
                return false; 
        }
        
        if( !SetPixelFormat(dc, PixelFormat, &pfd) ) { 
                MessageBox(hwndMain, "Pixel Format could not be set...", "Error", MB_OK); 
                return false; 
        }
	            
        if( 0 == (rc = wglCreateContext(dc)) ) {
                MessageBox(hwndMain, "Rendering Context could not be created...", "", MB_OK);
                return false;
        }
        
        wglMakeCurrent(dc, rc);
        
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewport);
    
        return true;
}

void WorkerThread::closeGl()
{
    	if( dc != 0 )											
		{
        	wglMakeCurrent(dc, 0);							
			if( rc != 0 ) { 										
				wglDeleteContext(rc);							
				rc = 0;									
			}
			ReleaseDC(hwndGl, dc);					
			dc = 0;											
		}
}

void WorkerThread::exportCurrAnimation(qThreadMessage *m)
{
    if( !theAnimations.empty() ) 
    {
        expAnimation = bAbortLocked = bWorking = playingAnimation = true;
        if( initGlExport(m->rect_data.right, m->rect_data.bottom) ) 
        {
            char buffer[512];
            char *extension;
            
            extension = strrchr(m->str_data,'.');
            if(extension)
            	*extension = '\0';
            
            theAnimationsList::iterator it = theAnimations.begin();
            // somehow list::iterator does not suppoer it+=.. - in vc6 it does
            
            // -- GNU is right, vc6 is wrong again
            // operator += should only be supported if it works in constant time
            // for lists, it can't. - WTT
            
            for(int xx=0; xx<currAnimation; xx++) it++;
            int frames = (*it)->GetFrameCount();
            for(int i=0; i<frames; i++) 
            {
                (*it)->SetFrame(i);
                sprintf(buffer, "%s%03d.png", m->str_data, i+1);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                exportGlBitmap(buffer);
                setAnimationInfo(currAnimation, theAnimations.size(), i, frames);
            }       
            closeGlExport();
        }
        expAnimation = bAbortLocked = bWorking = playingAnimation = false;
    }

};

void WorkerThread::exportAnimations(qThreadMessage *m)
{
    if( !theAnimations.empty() ) 
    {
        expAnimation = bAbortLocked = bWorking = playingAnimation = true;
        if( initGlExport(m->rect_data.right, m->rect_data.bottom) ) 
        {
            char buffer[512];
            char *extension;
            
            extension = strrchr(m->str_data,'.');
            if(extension)
            	*extension = '\0';

            theAnimationsList::iterator it = theAnimations.begin();
            int j=0;
            while(it != theAnimations.end())
            {
                int frames = (*it)->GetFrameCount();
                for(int i=0; i<frames; i++) 
                {
                    (*it)->SetFrame(i);
                    sprintf(buffer, "%s%d-%03d.png", m->str_data, j+1, i+1);
                    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                    exportGlBitmap(buffer);
                    setAnimationInfo(j, theAnimations.size(), i, frames);
                }
                it++;
                j++;
            }
            closeGlExport();
        }         
        expAnimation = bAbortLocked = bWorking = playingAnimation = false; 
    }      
};

// initializes an offscreen dc for high-res rendering
bool WorkerThread::initGlExport(int w, int h)
{    
    exp_w = w;
    exp_h = h;
    swapBuffers();
        
    // save dimensions
    //int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
         
    int PixelFormat;
        
    // memory dc
    mdc = CreateCompatibleDC(dc);
    if( mdc == NULL ) { 
        MessageBox(hwndMain, "mdc = NULL", "Error", MB_OK); 
        return false; 
    }
    
    unsigned char *pixels;
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    int bpp = 24;
    DWORD dwSizeImage = w * h * bpp / 8;
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiHeader.biSizeImage = dwSizeImage;
    bi.bmiHeader.biXPelsPerMeter = 2952;
    bi.bmiHeader.biYPelsPerMeter = 2952;
    HBITMAP hBmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, (void **)&pixels, 0, 0);
    if( hBmp == NULL ) { 
        MessageBox(hwndMain, "hBmp = NULL", "Error", MB_OK); 
        return false;
    }
        
    HBITMAP hBmpOld = (HBITMAP)SelectObject(mdc, hBmp);
    if( hBmpOld == NULL ) {
        MessageBox(hwndMain, "hBmpOld = NULL", "Error", MB_OK);
        return false;
    }
   
    // memory dc
    pfd.dwFlags =  PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
    pfd.cColorBits = 24;
    PixelFormat = ChoosePixelFormat(mdc, &pfd);
    if( PixelFormat == 0 ) { 
        MessageBox(hwndMain, "Error getting Gl Pixel Format", "Error", MB_OK); 
        return false; 
    }
        
    if( !SetPixelFormat(mdc, PixelFormat, &pfd) ) { 
            MessageBox(hwndMain, "Pixel Format could not be set...", "Error", MB_OK); 
            return false;
    }
     
    if( 0 == (mrc = wglCreateContext(mdc)) ) {
            MessageBox(hwndMain, "Rendering Context could not be created...", "Error", MB_OK);
            return false;
    }
        
    wglMakeCurrent(mdc, mrc);
         
    SetGLObjectsDirty();
    reshape(w,h);
    SetNeedsReInit();
    
    return true;
}

// closes it..
void WorkerThread::closeGlExport()
{
    // reset
    wglMakeCurrent(dc, rc);
    
    // delete stuff
    wglDeleteContext(mrc);
    mrc = NULL;
    DeleteObject(hBmp);    
    hBmp = NULL;
    DeleteDC(mdc);
    mdc = NULL;
    
    // reset qgl
    SetGLObjectsDirty();
	reshape(vp[2],vp[3]);
    SetNeedsReInit();
    display();
}

bool WorkerThread::exportGlBitmap(char *name)
{
        SetProgressMessage("Exporting...");
        StartDeterminateProgress();
        
        display();
           
        // showing a simple preview
        if( expAnimation ) {
             float v = float(vp[3]) / float(vp[2]);
            StretchBlt(dc, 0, 0, vp[2], vp[3], mdc, 0, 0, exp_w, int(exp_w * v), SRCCOPY); 
        }
        ExportImage(name);
        
        return true;
}

// invokes parser...
bool WorkerThread::load(const std::string &fname)
{
        if( bFileLoaded ) {
                ReleaseDescription();
                bFileLoaded = false;
        }
    
        AutoreleasePool::PushNewPool();
	    VisualObject::BSP = new BSPTree();
        VisualObject::BSPMaterialUpdater = new MaterialUpdater();
        VisualObject::BSPMaterialUpdater -> DependOn(VisualObject::BSP);
        InitCubeInfo();
        RegisterFieldFunctions();
        RegisterStandardFunctions();
        InitAngleVariables();
  
        worldXRange = worldYRange = worldZRange = fullRange;
	
        curPosition.line = 1;
	
        SendDlgItemMessage(hwndDialog, IDC_VARIABLE_LIST, LB_RESETCONTENT,
                                                0, 0);
        checkMessages();
        
        try
        {	
                yyparse();
                AutoreleasePool::FlushCurrentPool();
                DetermineWorldSize();
        }    
        catch(DescError& e)
        {
		        char buffer[256];
                sprintf(buffer, "Parsing file failed - %s, line: %d", e.message.c_str(), e.pos.line);        
                MessageBox(hwndMain, buffer, "Error", MB_OK); 
                AutoreleasePool::FlushCurrentPool();
                ReleaseDescription();
                return false;
        }
        catch(exception& e)
        {
		        char buffer[256];
                sprintf(buffer, "Parsing file failed - %s", e.what());        
                MessageBox(hwndMain, buffer, "Error", MB_OK); 
                AutoreleasePool::FlushCurrentPool();
                ReleaseDescription();
                return false;
        }
        catch(...)
        {
                MessageBox(hwndMain, "Unknown Exception", "Error", MB_OK); 
                AutoreleasePool::FlushCurrentPool();
                ReleaseDescription();
                return false;
        }  
        
        
        definedRealVariablesMap::iterator currentVar = definedRealVariables.begin();
		while( currentVar != definedRealVariables.end() ) 
		{
          if( !currentVar->second->constant ) 
                             SendDlgItemMessage(hwndDialog, IDC_VARIABLE_LIST, LB_ADDSTRING,
                                                0, (LPARAM)currentVar->first.c_str());
          currentVar++;  
        }
        
        setAnimationInfo(0, theAnimations.size(), 0, theAnimations.size() > 0 ? 
                    theAnimations.front()->GetFrameCount() : 0);
        
        bFileLoaded = true;
};
