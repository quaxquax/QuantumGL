#import "QuantumView.h"
//#import "WorkerThreadController.h"
#include "QuantumController.h"

#include <OpenGL/gl.h>

#include "QuantumConfig.h"
#include "QuantumFrontend.h"

@implementation QuantumView

- (id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
	context = nil;
	showingSomething = false;
	
	[[NSNotificationCenter defaultCenter]
		addObserver:self
		selector:@selector(reshape:)
		name:NSViewFrameDidChangeNotification
		object:self];
		
	glLock = [[NSLock alloc] init];
    return self;
}

/*- (void)setWorkerThread: (id<WorkerThreadMethods>) ws
{
	workerThread = ws;
}*/

#define SOFTWARE_MODE 0

- (NSOpenGLContext*) context
{
	if(!context)
	{
		NSOpenGLPixelFormatAttribute attrs[] = {
			NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute) 16,
#if SOFTWARE_MODE
			NSOpenGLPFAOffScreen,
#else
			NSOpenGLPFAAccelerated,
#endif
			(NSOpenGLPixelFormatAttribute) 0};
		
		pixFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
		
		context = [[NSOpenGLContext alloc] initWithFormat: pixFormat shareContext: nil];
		[context setView: self];
		SetNeedsReInit();
	}
	return context;
}

- (void)setupContext
{
	[[self context] makeCurrentContext];
}

- (void)releaseContext
{
	if(context)
		[context release];
	context = nil;
	if(pixFormat)
		[pixFormat release];
	pixFormat = nil;
}

- (void)dealloc
{
	if(context)
		[context release];
	if(pixFormat)
		[pixFormat release];
	[[NSNotificationCenter defaultCenter]
		removeObserver:self
		name:NSViewFrameDidChangeNotification
		object:self];
}

- (void)reshape:(NSNotification*)note
{
	NSLog(@"resized");
	NSRect r = [self bounds];
	//glViewport(0,0,(GLint) r.size.width,(GLint) r.size.height);
	/*[[self context] makeCurrentContext];
	[[self context] update];*/
	//[workerThread reshapeTo:r.size];
	
	[self lockGL];
	
		// ### assumes no state dependency
	reshape((GLint)r.size.width,(GLint)r.size.height);
	[self unlockGL];
}

- (void)lockGL
{
	NSLog(@"trying to lock...");
	[glLock lock];
	[self setupContext];
	NSLog(@"locked");
}
- (void)unlockGL
{
	NSLog(@"about to unlock...");
	[NSOpenGLContext clearCurrentContext];
	[glLock unlock];
	NSLog(@"unlocked");
}

- (void)displayIfNeeded
{
	
}

- (void)drawRect: (NSRect) r
{
//	[context update];
	
	BOOL displaySomethingElse = YES;
		
	if(showingSomething)
	{
		if([[QuantumController controller] tryLockUpdatedState])
		{
			[self lockGL];
			//[self setupContext];
			[[self context] update];
			display();
			[[QuantumController controller] unlockState];
			displaySomethingElse = NO;
		}
	}
		
	if(displaySomethingElse)
	{
		[self lockGL];
		//[self setupContext];
		[[self context] update];

		glClear(GL_COLOR_BUFFER_BIT);//  | GL_DEPTH_BUFFER_BIT);
	
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		glColor3f(1,1,0);
		glBegin(GL_LINES);
		glVertex2f(-1,-1);
		glVertex2f(1,1);
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	glFinish();
	[self unlockGL];
}

- (void)mouseDown:(NSEvent *)theEvent
{
	if(!showingSomething) return;
	NSPoint p = [theEvent locationInWindow];
	int button;
	switch([theEvent buttonNumber])
	{
		case 0: button = kMBLeft; break;
		case 1: button = kMBMiddle; break;
		case 2: button = kMBRight; break;
	}
	if(![[QuantumController controller] tryLockState])
		return;
	mouse(button,kMBDown,(int)p.x,(int)([self bounds].size.height - p.y));
	[[NSNotificationCenter defaultCenter] postNotificationName: @"VariableChanged" object: [QuantumController controller]];
	[[QuantumController controller] unlockState];		
}

- (void)mouseUp:(NSEvent *)theEvent
{
	if(!showingSomething) return;
	NSPoint p = [theEvent locationInWindow];
	int button;
	switch([theEvent buttonNumber])
	{
		case 0: button = kMBLeft; break;
		case 1: button = kMBMiddle; break;
		case 2: button = kMBRight; break;
	}
//	[[QuantumController controller] lockStateNow];
	if(![[QuantumController controller] tryLockState])
		return;
	mouse(button,kMBUp,(int)p.x,(int)([self bounds].size.height - p.y));
	[[NSNotificationCenter defaultCenter] postNotificationName: @"VariableChanged" object: [QuantumController controller]];
	[[QuantumController controller] unlockState];		
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	if(!showingSomething) return;
	NSPoint p = [theEvent locationInWindow];
//	[[QuantumController controller] lockStateNow];
	if(![[QuantumController controller] tryLockState])
		return;
	motion((int)p.x,(int)([self bounds].size.height - p.y));
	[[NSNotificationCenter defaultCenter] postNotificationName: @"VariableChanged" object: [QuantumController controller]];
	[[QuantumController controller] unlockState];		
}

@end
