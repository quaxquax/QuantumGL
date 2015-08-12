#import "AnimationController.h"
#import "QuantumController.h"

#include "Animator.h"
#include "QuantumDescription.h"

@implementation AnimationController

- (void) setupNumberOfFrames
{
	int n = selectedAnimation->GetFrameCount();

	NSLog(@"new max: %d",n);

	[positionSlider setMaxValue: n];
	[stepperButtons setMaxValue: n];

	[frameNumberFormatter setMinimum:
		[NSDecimalNumber decimalNumberWithDecimal:
			[[NSNumber numberWithInt: 1] decimalValue]]];
	[frameNumberFormatter setMaximum:
		[NSDecimalNumber decimalNumberWithDecimal:
			[[NSNumber numberWithInt: n] decimalValue]]];
}

- (IBAction)openAnimationPalette:(id)sender
{
	if(![[QuantumController controller] isQglOpen])
		return;
	if(theAnimations.begin() == theAnimations.end())
		return;
	[self buildAnimationMenu: animationMenu];
	[animationPopup selectItemAtIndex: 0];
	selectedAnimation = *theAnimations.begin();
	[[QuantumController controller] setActiveAnimation: selectedAnimation];
	[self setupNumberOfFrames];
	[self setCurrentFrame: 1];
	[animationPalette makeKeyAndOrderFront:sender];
}

- (void) windowDidBecomeKey: (NSNotification*) note
{
}

- (void) windowWillClose: (NSNotification*) note
{
	[self pauseAnimation];
	[[QuantumController controller] setActiveAnimation: nil];
}

- (IBAction)playPause:(id)sender
{
	//[[NSApplication sharedApplication] runModalForWindow: progressWindow];
	NSLog(@"playPause: %d",[playPauseButton intValue]);
	
	if([playPauseButton intValue])
	{
		int pos = [positionSlider intValue];
		int n = (int)[positionSlider maxValue];
		if(pos < n)
			[self frameUpdated];
		else
			[self setCurrentFrame: 1];
	}
}

- (void)frameUpdated
{
	if([playPauseButton intValue])
	{
		// ### if rendering anim: render frame
	
		int pos = [positionSlider intValue];
		int n = (int)[positionSlider maxValue];
		if(pos+1 <= n)
			[self setCurrentFrame: pos+1];
		else
			[self pauseAnimation];
	}
}

- (void)pauseAnimation
{
	if([playPauseButton intValue])
	{
		[playPauseButton setIntValue: 0];
		[self playPause: self];
	}
}

- (void)setCurrentFrame:(int) frame
{
	NSLog(@"go to frame %d",frame);
	[positionSlider setIntValue:frame];
	[stepperButtons setIntValue:frame];
	[frameTextField setIntValue:frame];
	
	[[QuantumController controller] 
							changeVariableNamed: "@anim_frame"
							toValue: frame-1];
}

- (IBAction)valueChanged:(id)sender
{
	[self pauseAnimation];
	int i = [sender intValue];
	if(i >= 1 && i <= [positionSlider maxValue])
		[self setCurrentFrame: i];
	else
		[self setCurrentFrame: [positionSlider intValue]];
}

- (IBAction)chooseAnimation:(id)sender
{
	NSLog(@"choose anim");
	[self setCurrentAnimationByIndex: [animationPopup indexOfSelectedItem]];
}

- (Animator*)getAnimationWithIndex:(int) index
{
	theAnimationsList::iterator p = theAnimations.begin();
	advance(p,index);
	return *p;
}

- (void)setCurrentAnimationByIndex:(int)index
{
	[animationPopup selectItemAtIndex: index];
	[self pauseAnimation];
	selectedAnimation = [self getAnimationWithIndex:index];
	[[QuantumController controller] setActiveAnimation: selectedAnimation];
	[self setupNumberOfFrames];
	[self setCurrentFrame: 1];
}

- (void)buildAnimationMenu: (NSMenu*) menu
{
	int nAnimations = theAnimations.size();
	int nItems = [menu numberOfItems];
	
	if(nItems < nAnimations)
	{
		// add some
		for(int i=nItems+1;i <= nAnimations;i++)
			[menu
				addItemWithTitle: [NSString stringWithFormat: @"Animation %d", i]
				action: nil
				keyEquivalent: @""];
	}
	else if(nItems > nAnimations)
	{
		// remove some
		for(int i=nAnimations;i<nItems;i++)
			[menu removeItemAtIndex:nAnimations];
	}
}

- (IBAction)abortRendering:(id)sender
{
	[[NSApplication sharedApplication] stopModal];
	[progressWindow close];
	[self pauseAnimation];
}

- (IBAction)dlgGo:(id)sender
{
	NSString *path = [[dlgOutputPath stringValue] stringByExpandingTildeInPath];
	BOOL dir;
	
	if(![[NSFileManager defaultManager] fileExistsAtPath: path isDirectory: &dir])
	{
		if([[NSFileManager defaultManager] createDirectoryAtPath: path attributes: nil])
			dir = YES;
		else
		{
			[dlgOutputPath selectText:nil];
			NSBeep();
			return;
		}
	}
	if(!dir)
	{
		[dlgOutputPath selectText:nil];
		NSBeep();
		return;
	}
	
	if([dlgSubrangeMatrix selectedRow] == 1)
	{
		Animator *anim = [self getAnimationWithIndex: [dlgAnimationPopup indexOfSelectedItem]];
		int n = anim->GetFrameCount();
		int from = [dlgFrom intValue];
		int to = [dlgTo intValue];
		if(from < 1 || from > n)
		{
			[dlgFrom selectText:nil];
			NSBeep();
			return;
		}
		if(to < 1 || to > n)
		{
			[dlgTo selectText:nil];
			NSBeep();
			return;
		}
	}
	[dlgPanel performClose: sender];
	[self setCurrentFrame: 0];
	[[NSApplication sharedApplication] stopModalWithCode:1];
}
- (IBAction)dlgCancel:(id)sender
{
	[[NSApplication sharedApplication] stopModalWithCode:0];
}
- (IBAction)dlgBrowse:(id)sender
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setCanChooseDirectories: YES];
	[panel setCanChooseFiles: NO];
	[panel setAllowsMultipleSelection: NO];
	
	if([panel respondsToSelector: @selector(_newFolder:)])
	{
		[panel setAccessoryView: newFolderAccessory];
		[newFolderButton setTarget: panel];
		[newFolderButton setAction: @selector(_newFolder:)];
	}
	
	if([panel runModalForTypes:nil] == NSOKButton)
	{
		[dlgOutputPath setStringValue: [panel filename]];
	}
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	return [[QuantumController controller] isQglOpen] && theAnimations.size() != 0;
}

- (IBAction)exportAnimation:(id)sender
{
	//[dlgOutputPath setStringValue:@"/Users/wolfgang/animation1"];
	//[dlgOutputName setStringValue:@"animation"];
	
	[self buildAnimationMenu: dlgAnimationMenu];
	[dlgAnimationPopup selectItemAtIndex: [animationPopup indexOfSelectedItem]];
	[self dlgUpdateNrOfFrames:nil];
	int code = [[NSApplication sharedApplication] runModalForWindow: dlgPanel];
	[dlgPanel close];
	if(code == 1)
	{
		[self setCurrentAnimationByIndex: [dlgAnimationPopup indexOfSelectedItem]];
		int n = selectedAnimation->GetFrameCount();
		[playPauseButton setIntValue: 0];
		
		NSString *path = [[dlgOutputPath stringValue] stringByExpandingTildeInPath];
		NSString *baseName = [dlgOutputName stringValue];
		
		int width = [dlgWidth intValue];
		int height = [dlgHeight intValue];
		
		[progressBar setDoubleValue: 0];
		[progressMessage setStringValue: @""];
		
		int from = 1;
		int to = n;
		
		if([dlgSubrangeMatrix selectedRow] == 1)
		{
			from = [dlgFrom intValue];
			to = [dlgTo intValue];
		}
		
		NSModalSession session =
			[[NSApplication sharedApplication] 
				beginModalSessionForWindow:progressWindow];
		for(int i=from;i<=to;i++)
		{
			NSString *filename = [path stringByAppendingPathComponent:
								[NSString stringWithFormat: @"%@%03d.tiff", baseName, i]
							];

			[progressMessage setStringValue:
				[NSString stringWithFormat: @"Rendering frame %d of %d...", i, n]];
			if([[NSApplication sharedApplication] runModalSession: session]
				== NSRunStoppedResponse)
				break;

			NSLog(@"frame name: %@",filename);
			[self setCurrentFrame: i];
			
				// ### ??? ### why why why is this necessary ??? 
			[[QuantumController controller] lockState];
			[[QuantumController controller] unlockState];

			if([[NSApplication sharedApplication] runModalSession: session]
				== NSRunStoppedResponse)
				break;

			[[QuantumController controller]
				exportImageToFile: filename
				withWidth: width
				andHeight: height];
			
			[progressBar setDoubleValue: ((double)(i-from))/(to-from+1)];
		}
		[[NSApplication sharedApplication] endModalSession:session];
		[progressWindow close];
		//[self setCurrentFrame: i];
		//[[NSApplication sharedApplication] runModalForWindow: progressWindow];
	}
}

- (IBAction)subrangeMatrixAction:(id)sender
{
	BOOL en = [dlgSubrangeMatrix selectedRow] == 1;
	[dlgFrom setEnabled:en];
	[dlgTo setEnabled:en];
}

- (IBAction)dlgUpdateNrOfFrames:(id)sender
{
	Animator *anim = [self getAnimationWithIndex: [dlgAnimationPopup indexOfSelectedItem]];
	int n = anim->GetFrameCount();
	
	[dlgTotal setIntValue: n];
	[frameNumberFormatter setMinimum:
		[NSDecimalNumber decimalNumberWithDecimal:
			[[NSNumber numberWithInt: 1] decimalValue]]];
	[frameNumberFormatter setMaximum:
		[NSDecimalNumber decimalNumberWithDecimal:
			[[NSNumber numberWithInt: n] decimalValue]]];
}

- (void) doClose
{
	[animationPalette performClose:nil];
}
@end

