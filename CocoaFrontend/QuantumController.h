//
//  QuantumController.h
//  QuantumGL
//
//  Created by wolfgang on Sat Oct 20 2001.
//  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class QuantumView;
@class WorkerThreadController;
@protocol WorkerThreadMethods;
@class AnimationController;

const int kNormalCondition = 0;
const int kDirtyCondition = 1;
const int kBusyCondition = 2;

class Animator;

@protocol UIThreadMethods
- (oneway void) progressBusy;
- (oneway void) progressIdle;
- (oneway void) progressMessage: (in NSString *)str;
- (oneway void) progressDeterminate: (BOOL) det;
- (oneway void) progress: (double) x;
- (oneway void) descriptionLoaded;
- (oneway void) display;

- (oneway void) updateFinished;
- (oneway void) updateAborted;
- (oneway void) workerInitFinished: (id<WorkerThreadMethods>) workerThread;

- (oneway void) descriptionFailed: (NSString*) why;
@end

@interface QuantumController : NSObject <UIThreadMethods> {
	QuantumView	*oglView;
	NSWindow	*oglWindow;
	NSPanel		*progressPanel;
	NSProgressIndicator	*progressBar;
	NSTextField	*progressMessage;
	NSPanel		*varPanel;
	NSTableView	*varTable;
	NSView		*exportSavePanelView;
	NSTextField	*exportSavePanelWidth;
	NSTextField	*exportSavePanelHeight;
	NSSlider	*exportSavePanelAASlider;
	NSTextField	*exportSavePanelAAStaticText;
	
	//id<WorkerThreadMethods>	workerThread;
	
	id<WorkerThreadMethods>	workerThreadProxy;
	BOOL		abortUpdating;
	int			preventAbort;
	//NSConditionLock	*stateLock;
	
	BOOL		isOpen;
	BOOL		updating;
	BOOL		dirty;
	BOOL		exporting;
	
	NSConnection	*workerConnection;
	
	NSMutableDictionary	*varChanges;
	
	Animator	*activeAnimation;
	AnimationController *animationController;
	
	NSString *descriptionFileName;
}

- (void)applicationWillFinishLaunching: (id)note;

- (IBAction)selectAndOpenDescriptionFile: (id)sender;

- quantumView;

+ controller;


- (BOOL) tryLockUpdatedState;
- (void) lockUpdatedState;
- (BOOL) tryLockState;
- (void) lockState;
- (void) lockStateNow;
- (void) unlockState;
- (void) unlockUpdatedState;
- (void) unlockDirtyState;
- (void) forceUpdateState;


- (void) changeVariableNamed: (const char*) n toValue: (double) x;
- (void) delayedChangeVariableNamed: (const char*) n toValue: (double) x;


- (void) progressBusy;
- (void) progressIdle;
- (void) progressMessage: (NSString *)str;
- (void) progressDeterminate: (BOOL) det;
- (void) progress: (double) x;
- (void) descriptionLoaded;
- (void) descriptionFailed: (NSString*) why;
- (void) display;

- (void) updateFinished;
- (void) updateAborted;
- (void) workerInitFinished: (id<WorkerThreadMethods>) workerThread;

- (void) doExecuteVarChanges;
- (void) executeVarChanges;
//- (BOOL) tryChangeVariableNamed: (const char*) n toValue: (double) x;

- (void) exportImage: (id) sender;

- (void) setActiveAnimation: (Animator*) anim;

- (void)exportImageToFile: (NSString*) filename
		withWidth: (int) width
		andHeight: (int) height;
		
- (BOOL)isQglOpen;
@end
