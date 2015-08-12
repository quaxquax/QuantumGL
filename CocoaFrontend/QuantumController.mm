//
//  QuantumController.mm
//  QuantumGL
//
//  Created by wolfgang on Sat Oct 20 2001.
//

#import "QuantumController.h"
#import "QuantumView.h"
#import "WorkerThreadController.h"
#import "AnimationController.h"

#include "QuantumConfig.h"
#include "QuantumFrontend.h"
#include "QuantumDescription.h"
#include "RealVariable.h"
#include "Animator.h"
#include "VisualObject.h"
#include "BSPTree.h"
#include "AutoreleasePool.h"
#include "FieldFunctionUser.h"
#include "StandardFunctions.h"
#include "CubeInfo.h"
#include "QuantumProgress.h"
#include "LockGL.h"

#include <QuickTime/QuickTime.h>

QuantumController *qcontroller = nil;
QuantumView *qview = nil;

@implementation QuantumController

/*- (void)sendReadDesc:(NSString*) filename
{
	if(workerThread == nil)
	{
		NSLog(@"nil worker thread");
		NSRunLoop *run = [NSRunLoop currentRunLoop];
		[run runUntilDate: [NSDate dateWithTimeIntervalSinceNow: 1]];
		[run performSelector:@selector(sendReadDesc:)
				target:self argument:filename
				order:100000
				modes:[NSArray arrayWithObjects: 
							NSDefaultRunLoopMode,
							NSModalPanelRunLoopMode,
							NSEventTrackingRunLoopMode,nil]
				];
	}
	else
	{
		[workerThread loadDescriptionFile: filename];
		
	}
}*/


- (id)init
{
	NSLog(@"init");
	qcontroller = self;

	
	abortUpdating = NO;
	preventAbort = 0;
	
	dirty = NO;
	updating = NO;
	workerThreadProxy = nil;
	varChanges = nil;
	exporting = NO;
	isOpen = NO;
	
	activeAnimation = NULL;
	
	descriptionFileName = nil;
	
	return self;
}

- (void)applicationWillFinishLaunching: (id)sender
{	
	qview = oglView;

	NSPort *port1;
	NSPort *port2;
	NSArray *paramArray;

	port1 = [NSPort port];
	port2 = [NSPort port];

	workerConnection = [[NSConnection alloc] initWithReceivePort:port1
																sendPort:port2];
	[workerConnection setRootObject:self];
	
	[workerConnection addRequestMode: NSEventTrackingRunLoopMode];

	paramArray = [NSArray arrayWithObjects:port2, port1, self, nil];

	//updating = YES;
	//dirty = YES;

	[NSThread detachNewThreadSelector:@selector(workerThreadMain:)
			toTarget:[WorkerThreadController class]
			withObject:paramArray];
			
	/*[self workerInitFinished:
		[[WorkerThreadController alloc] initWithController: self andProxy: self]];*/

}

- (BOOL)applicationOpenUntitledFile:(NSApplication *)theApplication
{
	[self selectAndOpenDescriptionFile: self];
	return YES;
}

- (BOOL)applicationShouldHandleReopen:(NSApplication *)theApplication hasVisibleWindows:(BOOL)flag
{
	if(!isOpen && !updating && [theApplication modalWindow] == nil)
		return YES;
	else	
		return NO;
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	if(updating)
		return NO;
	
	while(!workerThreadProxy)
	{
		NSLog(@"loop...");
		[[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode
									beforeDate: [NSDate dateWithTimeIntervalSinceNow: 2]];
		NSLog(@"...loop");
	}
	updating = YES;
	dirty = YES;
	[oglWindow setTitleWithRepresentedFilename: filename];
	
	if([self isQglOpen])
	{
		//###[workerThreadProxy releaseDescription];
		[oglWindow close];
		//[varPanel performClose:nil];
		//[animationController doClose];
		//[oglView releaseContext];
	}
	if(descriptionFileName)
		[descriptionFileName release];
	descriptionFileName = [filename retain];
	
	[workerThreadProxy loadDescriptionFile: filename];
	return YES;
}

- (void)openPanelDidEnd:(NSOpenPanel *)sheet returnCode:(int)returnCode contextInfo:(void  *)contextInfo
{
	if(returnCode == NSOKButton)
	{
		NSLog(@"openPanelDidEnd");
		NSString *filename = [sheet filename];
		NSAssert(filename,@"no filename?");
		
		[self application:[NSApplication sharedApplication] openFile: filename];
	}
	 
	   //[self loadDescriptionFile: filename];
}

- (void)selectAndOpenDescriptionFile: (id)sender
{
	NSOpenPanel *sheet = [NSOpenPanel openPanel];
	[sheet setAllowsMultipleSelection: NO];
	
	/*[sheet beginSheetForDirectory: nil
		file: nil
		types: [NSArray arrayWithObject: @"qgl"]
		modalForWindow: oglWindow
		modalDelegate: self
		didEndSelector: @selector(openPanelDidEnd:returnCode:contextInfo:)
		contextInfo: nil];*/
	int returnCode = [sheet runModalForDirectory: nil
		file: nil
		types: [NSArray arrayWithObject: @"qgl"]];
	[self openPanelDidEnd: sheet returnCode: returnCode contextInfo:nil];
}

- (void)windowWillClose: (NSNotification*) notification
{
	NSLog(@"window will close");
	NSAssert(workerThreadProxy != nil,@"worker thread");
	
	isOpen = NO;
	[varPanel performClose:nil];
	[animationController doClose];
	[workerThreadProxy releaseDescription];
	[oglView releaseContext];
	NSLog(@"window will close: end");
}

- (BOOL)isQglOpen
{
	return isOpen;
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	SEL action = [menuItem action];
	if(action == @selector(selectAndOpenDescriptionFile:))
		return YES;//![self isQglOpen];
	else	//  all others
		return [self isQglOpen];
}

/*
- (void)setWorkerThreadController: (id)contr
{
	NSLog(@"worker thread arrived");
	workerThread = [contr retain];
	[workerThread setProtocolForProxy:@protocol(WorkerThreadMethods)];
	[oglView setWorkerThread: contr];
}*/

- quantumView;
{
	return oglView;
}

+ controller
{
	return qcontroller;
}

/*- (id<WorkerThreadMethods>) workerThread
{
	return workerThread;
}*/

- (void) workerInitFinished: (id<WorkerThreadMethods>) workerThread
{
	[(id)workerThread setProtocolForProxy:@protocol(WorkerThreadMethods)];
	workerThreadProxy = [(id)workerThread retain];
	NSLog(@"workerInitFinished: %x", (int)workerThreadProxy);
	//[workerThreadProxy testMessage];
}

- (void) updateFinished
{
	updating = NO;
	dirty = NO;
	NSLog(@"updateFinished.");
	//[oglView setNeedsDisplay: YES];
	[self display];
	[self executeVarChanges];
	/*[[NSRunLoop currentRunLoop] 
			performSelector:@selector(executeVarChanges)
			target: self
			argument: nil
			order: NSUpdateWindowsRunLoopOrdering + 1
			modes: [NSArray arrayWithObjects:
				NSDefaultRunLoopMode,
				NSEventTrackingRunLoopMode,
				nil]
		];*/
	if(!updating && !dirty)
	{
		[animationController frameUpdated];
	}
}

- (void) updateAborted
{
	updating = NO;
	NSLog(@"updateAborted.");
}

- (void) lockUpdatedState
{
	//[stateLock lockWhenCondition: kNormalCondition];
	
	while(!workerThreadProxy || updating || dirty)
	{
		NSLog(@"loop...");
		[[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode
									beforeDate: [NSDate dateWithTimeIntervalSinceNow: 2]];
		NSLog(@"...loop");
	}
}

- (BOOL) tryLockUpdatedState
{
	return !dirty && !updating && workerThreadProxy != nil;
	//return [stateLock tryLockWhenCondition: kNormalCondition];
}

- (void) lockStateNow
{
	//NSLog(@"initiating abort");
/*	abortUpdating = YES;
	[stateLock lock];
	if(abortUpdating)
		//NSLog(@"no abort was necessary");
	abortUpdating = NO;*/
	
	abortUpdating = YES;
	while(!workerThreadProxy || updating)
	{
		NSLog(@"loop...");
		NSString * mode = [[NSRunLoop currentRunLoop] currentMode];
		if(!mode)
			mode = NSDefaultRunLoopMode;
		NSLog(@"mode: %@",mode);
		[[NSRunLoop currentRunLoop] runMode: mode /*NSDefaultRunLoopMode*/
									beforeDate: [NSDate dateWithTimeIntervalSinceNow: 2]];
		NSLog(@"...loop");
	}
	abortUpdating = NO;
}

- (void) lockState
{
	while(!workerThreadProxy || updating)
	{
		NSLog(@"loop...");
		NSString * mode = [[NSRunLoop currentRunLoop] currentMode];
		if(!mode)
			mode = NSDefaultRunLoopMode;
		NSLog(@"mode: %@",mode);
		[[NSRunLoop currentRunLoop] runMode: mode /*NSDefaultRunLoopMode*/
									beforeDate: [NSDate dateWithTimeIntervalSinceNow: 2]];
		NSLog(@"...loop");
	}
}

- (BOOL) tryLockState
{
	return !updating && workerThreadProxy != nil;
	//return [stateLock tryLockWhenCondition: kNormalCondition];
}

- (void) unlockState
{
	//[stateLock unlock];
	if(dirty)
	{
		updating = YES;
		[workerThreadProxy quantumUpdate];
	}
}
- (void) unlockUpdatedState
{
	dirty = NO;
	//[stateLock unlockWithCondition: kNormalCondition];
}

- (void) unlockDirtyState
{
	dirty = YES;
	updating = YES;
	//NSLog(@"proxy: %@", workerThreadProxy);
	[workerThreadProxy quantumUpdate];
	//[stateLock unlockWithCondition: kDirtyCondition];
}

- (void) forceUpdateState
{
	[self lockUpdatedState];
	[self unlockUpdatedState];
}

- (void) progressBusy
{
	[progressMessage setStringValue: @"Busy"];
	[progressBar startAnimation:self];
	[progressBar setIndeterminate: YES];
}

- (void) progressIdle
{
	[progressMessage setStringValue: @"Idle"];
	[progressBar stopAnimation:self];
	[progressBar setIndeterminate: NO];
	[progressBar setDoubleValue: 0];
}

- (void) progressMessage: (NSString*) msg
{
	[progressMessage setStringValue: msg];
}

- (void) progressDeterminate: (BOOL) det
{
	[progressBar setIndeterminate: !det];
}

- (void) progress: (double) x
{
	[progressBar setDoubleValue: x];
}

- (void) descriptionLoaded
{
	qview->showingSomething = YES;
	[varPanel orderFront: self];
	[varTable noteNumberOfRowsChanged];
	[oglWindow makeKeyAndOrderFront: nil];
	[[self quantumView] reshape:nil];
	isOpen = YES;
}

- (void) descriptionFailed: (NSString*) why
{
	[workerThreadProxy releaseDescription];
	NSRunAlertPanel([descriptionFileName lastPathComponent],
					why,@"OK", nil,nil);
	updating = dirty = NO;
}

- (void) display
{
	//[oglView setNeedsDisplay: YES];
	if([oglView lockFocusIfCanDraw])
	{
		[oglView drawRect: [oglView frame]];
		[oglView unlockFocus];
	}
}

- (void) changeVariableNamed: (const char*) n toValue: (double) x
{
/*	[self lockStateNow];
	definedRealVariablesMap::iterator p = definedRealVariables.find(string(n));
	p->second->ChangeTo(x);	
	[self unlockDirtyState];*/
	
	if(!varChanges)
		varChanges = [[NSMutableDictionary alloc] init];
	[varChanges setObject: [NSNumber numberWithDouble: x]
				forKey: [NSString stringWithCString: n]];
	[self lockStateNow];
	[self doExecuteVarChanges];
	NSLog(@"changeVariable: immediate update");
	[self unlockDirtyState];
}

- (void) delayedChangeVariableNamed: (const char*) n toValue: (double) x
{
	if(!varChanges)
		varChanges = [[NSMutableDictionary alloc] init];
	[varChanges setObject: [NSNumber numberWithDouble: x]
				forKey: [NSString stringWithCString: n]];
	if([self tryLockState])
	{
		[self doExecuteVarChanges];
		NSLog(@"delayedChangeVariable: immediate update");
		[self unlockDirtyState];
	}
	else
		NSLog(@"delayedChangeVariable: varchange delayed");
}

- (void) doExecuteVarChanges
{
	if(varChanges)
	{
		NSEnumerator* keyEnum = [varChanges keyEnumerator];
		NSString* varName;
		while(varName = [keyEnum nextObject])
		{
			if([varName isEqualToString:@"@anim_frame"])
			{
				int frame = [[varChanges objectForKey: varName] intValue];
				if(activeAnimation)
					activeAnimation->SetFrame(frame);
			}
			else
			{
				definedRealVariablesMap::iterator p = definedRealVariables.find(string([varName cString]));
				p->second->ChangeTo([[varChanges objectForKey: varName] doubleValue]);	
			}
		}
		
		[varChanges release];
		varChanges = nil;
		[[NSNotificationCenter defaultCenter] postNotificationName: @"VariableChanged" object: self];
	}
}

- (void) executeVarChanges
{
	if(varChanges)
	{
		if([self tryLockState])
		{
			[self doExecuteVarChanges];
			NSLog(@"executeVarChanges: delayed var change");
			[self unlockDirtyState];
		}
		else
			NSLog(@"!!! executeVarChanges");
	}
}
/*
- (BOOL) tryChangeVariableNamed: (const char*) n toValue: (double) x
{
	if([self tryLockState])
	{
		definedRealVariablesMap::iterator p = definedRealVariables.find(string(n));
		p->second->ChangeTo(x);	
		[self unlockDirtyState];
		return YES;
	}
	else
		return NO;
}
*/

- (void)exportImageToFile: (NSString*) filename
		withWidth: (int) width
		andHeight: (int) height
{
	NSOpenGLContext *context;
	NSOpenGLPixelFormat* pixFormat;

	NSLog(@"export: about to lock");
	[self lockUpdatedState];

	
	NSOpenGLPixelFormatAttribute attrs[] = {
		NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute) 16,
		NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute) 24,
		NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute) 0,
		NSOpenGLPFAOffScreen,
		(NSOpenGLPixelFormatAttribute) 0};
	
	pixFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
	context = [[NSOpenGLContext alloc] initWithFormat: pixFormat shareContext: nil];
	NSLog(@"context allocated: %@",context);
	
#if 0	
	GWorldPtr theWorld;
	Rect r;
	MacSetRect(&r,0,0,width,height);
	
	NewGWorld(&theWorld,32,&r,NULL,NULL,0);
	
	PixMapHandle thePixMap = GetGWorldPixMap(theWorld);
	LockPixels(thePixMap);
	
	int rowBytes = (*thePixMap)->rowBytes & 0x3FFF;
	Ptr buffer = (*thePixMap)->baseAddr;
#endif

	int rowBytes = width*4;
	void *buffer = malloc(rowBytes * height);
	memset(buffer,255,rowBytes*height);

	[context setOffScreen: buffer width:width height:height rowbytes:rowBytes];

	[context makeCurrentContext];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	NSLog(@"export: locked");
	exporting = YES;
	
	SetGLObjectsDirty();
	NSLog(@"export: about to update");
	UpdateVisualObjects();
	NSLog(@"export: about to reshape");

	reshape(width,height);
	NSLog(@"export: about to display");

	SetNeedsReInit();
	display();
	
	exporting = NO;

	SetGLObjectsDirty();
	[[self quantumView] reshape:nil];	// reshape back to window size

	NSLog(@"export: about to unlock");
	[self unlockDirtyState];	// ... or should it be dirty?
	
	[pixFormat release];
	[context release];

	NSLog(@"export: converting pixel format");
	unsigned char *planes[1];
	planes[0] = (unsigned char*) buffer;
	for(int v=0;v<height;v++)
	{
		unsigned char *p = ((unsigned char*)buffer) + v * rowBytes;
		for(int h=0;h<height;h++)
		{
			p[0] = p[1]; p++;
			p[0] = p[1]; p++;
			p[0] = p[1]; p++;
			p[0] = 255; p++;
		}
	}
	NSLog(@"export: creating image rep");

	NSBitmapImageRep *rep = 
		[[NSBitmapImageRep alloc]
			initWithBitmapDataPlanes: planes
			pixelsWide: width
			pixelsHigh: height
			bitsPerSample: 8
			samplesPerPixel: 3
			hasAlpha: NO
			isPlanar: NO
			colorSpaceName: NSDeviceRGBColorSpace
			bytesPerRow: rowBytes
			bitsPerPixel: 32
		];
	NSLog(@"export: creating TIFF rep");
	NSData *data = [rep TIFFRepresentationUsingCompression: NSTIFFCompressionLZW
												factor: 1];
#if 0
	ComponentInstance exporterInstance = NULL;
	
	//if(exporterInstance == NULL)
	{
		OpenADefaultComponent(GraphicsExporterComponentType,'TIFF',&exporterInstance);
	//	GraphicsExportRequestSettings(exporterInstance,NULL,NULL);
	}
	GraphicsExportSetInputGWorld(exporterInstance,theWorld);
	
	Handle h = NewHandle(0);
	GraphicsExportSetOutputHandle(exporterInstance,h);
	
	GraphicsExportDoExport(exporterInstance,NULL);
	
	long size = GetHandleSize(h);
	NSLog(@"data size: %ld",size);
	HLock(h);
	NSData *data = [NSData dataWithBytes: *h length: size];
	DisposeHandle(h);
#endif
	NSLog(@"export: writing file");
	
	[data writeToFile: filename atomically: YES];
	[[NSFileManager defaultManager]
		changeFileAttributes: [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithUnsignedLong: 'TIFF'],
									NSFileHFSTypeCode,
									[NSNumber numberWithUnsignedLong: '8BIM'],
									NSFileHFSCreatorCode,
									nil]
		atPath: filename];
	
//	CloseComponent(exporterInstance);
//	DisposeGWorld(theWorld);
	free(buffer);
	[rep release];
}

- (void)exportImagePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void  *)contextInfo
{
	if(returnCode == NSOKButton)
	{
		NSString *filename = [sheet filename];
		NSAssert(filename,@"no filename?");
		
		int width = [exportSavePanelWidth intValue];//256;
		int height = [exportSavePanelHeight intValue];
	
		assert(width);
	
		[self exportImageToFile: filename
						withWidth: width
						andHeight: height];
	}
	//	NSLog(@"%@",[[NSFileManager defaultManager] fileAttributesAtPath: @"/Users/wolfgang/explode" traverseLink: NO]);
}

- (void) exportImage: (id) sender
{
	NSSavePanel *sheet = [NSSavePanel savePanel];
	[sheet setAccessoryView: exportSavePanelView];
	
	//[sheet setCanSelectHiddenExtension: YES];
	
	[sheet beginSheetForDirectory: nil
		file: nil
		modalForWindow: oglWindow
		modalDelegate: self
		didEndSelector: @selector(exportImagePanelDidEnd:returnCode:contextInfo:)
		contextInfo: nil];
	
}

- (void) setActiveAnimation: (Animator*) anim
{
	activeAnimation = anim;
}
@end

void ResizeDisplayTo(int w, int h)
{
}

void SwapBuffers()
{
}

void PostRedisplay()
{
	if(qcontroller)
		[qcontroller->oglView setNeedsDisplay:YES];
}

bool ShouldAbort()
{
	return qcontroller && qcontroller->abortUpdating && !qcontroller->preventAbort;
}

void CheckAbort() throw(AbortException)
{
	if(ShouldAbort())
		throw AbortException();
}

void LockAbort()
{
	qcontroller->preventAbort++;
}

void UnlockAbort()
{
	qcontroller->preventAbort--;
}

void LockGL()
{
	NSLog(@"LockGL");
	if(!qcontroller->exporting)
		[[qcontroller quantumView] lockGL];
}

void UnlockGL()
{
	if(!qcontroller->exporting)
		[[qcontroller quantumView] unlockGL];
}

bool ExportImage(const char * name)
{
	return true;
}