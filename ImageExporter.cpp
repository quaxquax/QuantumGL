#include "QuantumConfig.h"
#include "ImageExporter.h"

#include GL_GL_H

#ifdef __APPLE__
#include <QuickTime/QuickTime.h>
#else
#if QUANTUM_TARGET_WIN
#include <QTML.h>
#endif

#include <ImageCompression.h>
#include <QDOffscreen.h>
#include <TextUtils.h>
#endif

#include <iostream>
#include <cstdio>

using namespace std;

#if QUANTUM_TARGET_WIN
static bool QuickTimeInited = false;
#endif

ComponentInstance ci = NULL;
		StandardFileReply reply;

bool ExportBackBuffer(int w, int h, const char * name)
{
	char localName[256];
	StringPtr pName = NULL;
	if(name)
	{
		memcpy(localName+1,name,strlen(name));
		pName = (StringPtr) localName;
		pName[0] = strlen(name);
	}
#if QUANTUM_TARGET_WIN
	if(!QuickTimeInited)
	{
		cout << "Initializing QuickTime...\n";
		InitializeQTML(kInitializeQTMLNoSoundFlag);
		//EnterMovies();
		
		QuickTimeInited = true;
	}
#endif	
	GLubyte *imageData = new GLubyte [w * h * 3 + 4*h];
	
	cout << "Reading Image Data...\n";

	glPixelStorei(GL_PACK_ALIGNMENT,1);
	for(int i=w*h*3; i<w*h*3 + 4*h; i++)
		imageData[i] = 0xEB;
	glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,imageData);
	if(imageData[w*h*3] != 0xEB)
	{
		cout << "overwritten ";
		for(int i=w*h*3; i<w*h*3 + 4*h; i++)
			if(imageData[i] == 0xEB)
			{
				cout << i-w*h*3 << " bytes.\n";
				return false;
			}
		cout << " everything.\n";
	}

	cout << "Converting Image Data to QuickTime format...\n";
	GWorldPtr theWorld;
	Rect r;
	MacSetRect(&r,0,0,w,h);
	
	NewGWorld(&theWorld,32,&r,NULL,NULL,0);
	
	PixMapHandle thePixMap = GetGWorldPixMap(theWorld);
	LockPixels(thePixMap);
	
	int rowBytes = (*thePixMap)->rowBytes & 0x3FFF;
	GLubyte *base = (GLubyte*)(*thePixMap)->baseAddr;
	for(int y = 0;y < h; y++)
		for(int x = 0;x < w; x++)
		{
			GLubyte *p = base + x * 4 + (h-y-1) * rowBytes;
			GLubyte *q = imageData + x * 3 + y * 3 * w;
			p[1] = q[0];
			p[2] = q[1];
			p[3] = q[2];
		}

	delete [] imageData;

	{
		
		if(pName && (ci != NULL))
		{
#if QUANTUM_TARGET_WIN
			int i = reply.sfFile.name[0];
			for(;i > 0 && reply.sfFile.name[i]!='\\';i--)	
				;
			i++;
			memcpy(reply.sfFile.name+i, name, strlen(name));
			reply.sfFile.name[0] = i-1 + strlen(name);
#elif QUANTUM_TARGET_MAC
			FSMakeFSSpec(reply.sfFile.vRefNum,reply.sfFile.parID,pName,&reply.sfFile);
#else
	#error ???
#endif
			//memcpy(reply.sfFile.name,pName,pName[0]+1);
		}
		else
			StandardPutFile("\pSave As...", pName ? pName : "\puntitled.pic", &reply);
		if(!reply.sfGood)
			return false;

		if(ci == NULL)
		{
			OpenADefaultComponent(GraphicsExporterComponentType,'PICT',&ci);
			GraphicsExportRequestSettings(ci,NULL,NULL);
		}
		GraphicsExportSetInputGWorld(ci,theWorld);
		
		GraphicsExportSetOutputFile(ci,&reply.sfFile);
		
		cout << "Exporting...\n";
		
		GraphicsExportDoExport(ci,NULL);
		
		//CloseComponent(ci);
	}
	
	DisposeGWorld(theWorld);
	
	
	cout << "Done.\n";
	return true;
}
