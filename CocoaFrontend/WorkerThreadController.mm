//
//  WorkerThreadController.mm
//  QuantumGL
//
//  Created by wolfgang on Sun Oct 21 2001.
//  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
//

#import "WorkerThreadController.h"
#import "QuantumController.h"
#import "QuantumView.h"

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

#include <iostream>
using std::cout;
using std::endl;

id<UIThreadMethods> gUIThreadProxy;

@implementation WorkerThreadController
+ (void) workerThreadMain: (NSArray*) params
{
	//[stateLock lock];

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	NSConnection *serverConnection = [NSConnection
		connectionWithReceivePort:[params objectAtIndex:0]
		sendPort:[params objectAtIndex:1]];
		
	id<UIThreadMethods> proxy = [[serverConnection rootProxy] retain];
	[(id)proxy setProtocolForProxy:@protocol(UIThreadMethods)];
	
	QuantumController *controller = [params objectAtIndex: 2];
	WorkerThreadController *workerThreadController = 
		[[self alloc] initWithController: controller andProxy: proxy];

	[serverConnection setRootObject: workerThreadController];
	
	NSLog(@"sending workerInitFinished: %@", workerThreadController);
	[proxy workerInitFinished: workerThreadController];
	[[NSRunLoop currentRunLoop] run];
	[pool release];
}

- (void) quantumUpdate
{
	[uiThreadProxy progressBusy];
	BOOL aborted = NO;
	NSLog(@"updating...");
	UpdateVisualObjects();
	if(ShouldAbort())
	{
		NSLog(@"aborted.");
		aborted = YES;
	}
	else
		NSLog(@"done updating.");
	[uiThreadProxy progressIdle];
/*	if(!aborted)
		[[uiThreadProxy quantumView] setNeedsDisplay: YES];*/
	if(aborted)
		[uiThreadProxy updateAborted];
	else
		[uiThreadProxy updateFinished];
}

- initWithController: contr andProxy: proxy
{
	self = [super init];
	qController = contr;
	uiThreadProxy = proxy;
	gUIThreadProxy = proxy;
	return self;
}

- (void) loadDescriptionFile: (NSString*) filename
{
	LockAbort();
	[uiThreadProxy progressBusy];
	[uiThreadProxy progressMessage:
		[NSString stringWithFormat: @"Loading \"%@\"...", filename]];
	
	AutoreleasePool::PushNewPool();
	
	SetNeedsReInit();
	
	VisualObject::BSP = new BSPTree();
	VisualObject::BSPMaterialUpdater = new MaterialUpdater();
	VisualObject::BSPMaterialUpdater -> DependOn(VisualObject::BSP);
	
	InitCubeInfo();
	RegisterFieldFunctions();
	RegisterStandardFunctions();
	InitAngleVariables();

	worldXRange = worldYRange = worldZRange = fullRange;

	cout << "reading description...\n";
	
	extern FILE* yyin;
	extern int yyparse();
	extern void yyrestart(FILE *in);

	yyin = fopen([filename UTF8String],"r");
	if(yyin == NULL)
	{
		return;
	}
	yyrestart(yyin);
	
	curPosition.line = 1;
	
	volatile bool error = false;
	NSString * volatile failStr;
	try
	{
		yyparse();
		PrintReferenceCountingStatistics();
		AutoreleasePool::FlushCurrentPool();
		PrintReferenceCountingStatistics();
		
		DetermineWorldSize();

		//currentVar = definedRealVariables.begin();
	}
	catch(DescError& e)
	{
		cout << "********* ERROR ********\n";
		cout << e.message << endl;
		cout << "line " << e.pos.line << endl;
		error = true;
		failStr = [NSString stringWithFormat: @"%s at line %d", e.message.c_str(), e.pos.line];
		/*for(;;)
			;*/
	}
	catch(exception& e)
	{
		cout << "*********** EXCEPTION ***********\n";
		cout << e.what() << endl;
		error = true;
		failStr = [NSString stringWithCString: e.what()];
		/*for(;;)
			;*/
	}
	catch(...)
	{
		cout << "********* UNEXPECTED EXCEPTION ********\n";
		error = true;
		failStr = @"Unexpected Exception";
		/*for(;;)
			;*/
	}
	
	UnlockAbort();
	if(!error)
	{
		[uiThreadProxy descriptionLoaded];
		[self quantumUpdate];
	}
	else
	{
		[uiThreadProxy progressIdle];
		[uiThreadProxy descriptionFailed: failStr];
	}
}

- (void) releaseDescription
{
	NSLog(@"release description");
	ReleaseDescription();
	NSLog(@"release description end");
}
@end

void SetProgressMessage(string msg)
{
	[gUIThreadProxy progressMessage:
			[NSString stringWithCString: msg.c_str()]];
}

void StartDeterminateProgress()
{
	[gUIThreadProxy progressDeterminate: YES];
}

void EndDeterminateProgress()
{
	[gUIThreadProxy progressDeterminate: NO];
}

void Progress(double x)
{
	[gUIThreadProxy progress: x];
}

