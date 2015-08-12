#import <Cocoa/Cocoa.h>

//@protocol WorkerThreadMethods;

@interface QuantumView : NSView //NSOpenGLView
{
	BOOL showingSomething;
	NSOpenGLContext	*context;
	NSOpenGLPixelFormat *pixFormat;
	NSLock	*glLock;
	//id<WorkerThreadMethods>	workerThread;
}
- (id)initWithFrame:(NSRect)frameRect;
- (void)dealloc;
- (void)drawRect: (NSRect) r;
//- (void)setWorkerThread: (id<WorkerThreadMethods>) ws;
- (void)setupContext;
- (void)reshape:(NSNotification*)note;
- (void)releaseContext;

- (void)lockGL;
- (void)unlockGL;
@end
