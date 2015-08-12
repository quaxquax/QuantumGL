#import <Cocoa/Cocoa.h>

class Animator;

@interface AnimationController : NSObject
{
    IBOutlet id frameTextField;
    IBOutlet id playPauseButton;
    IBOutlet id positionSlider;
    IBOutlet id stepperButtons;
	IBOutlet id frameNumberFormatter;
	IBOutlet id animationPopup;
	IBOutlet id animationMenu;
	IBOutlet id animationPalette;
	
	IBOutlet id progressBar;
	IBOutlet id progressMessage;
	IBOutlet id progressWindow;
	
	IBOutlet id dlgPanel;
	IBOutlet id dlgAnimationMenu;
	IBOutlet id dlgAnimationPopup;
	IBOutlet id dlgOutputName;
	IBOutlet id dlgOutputPath;
	IBOutlet id dlgFrom;
	IBOutlet id dlgTo;
	IBOutlet id dlgTotal;
	IBOutlet id dlgSubrangeMatrix;
	IBOutlet id dlgWidth;
	IBOutlet id dlgHeight;
	
	IBOutlet NSView* newFolderAccessory;
	IBOutlet NSButton* newFolderButton;

	Animator	*selectedAnimation;
}
- (IBAction)openAnimationPalette:(id)sender;

- (IBAction)playPause:(id)sender;
- (IBAction)valueChanged:(id)sender;
- (IBAction)chooseAnimation:(id)sender;

- (IBAction)abortRendering:(id)sender;

- (void)buildAnimationMenu: (NSMenu*) menu;
- (void)setCurrentFrame:(int) frame;
- (void)frameUpdated;
- (void)pauseAnimation;

- (IBAction)dlgGo:(id)sender;
- (IBAction)dlgCancel:(id)sender;
- (IBAction)dlgBrowse:(id)sender;

- (IBAction)exportAnimation:(id)sender;

- (IBAction)subrangeMatrixAction:(id)sender;

- (Animator*)getAnimationWithIndex:(int)index;
- (void)setCurrentAnimationByIndex:(int)index;

- (IBAction)dlgUpdateNrOfFrames:(id)sender;

- (void) doClose;
@end
