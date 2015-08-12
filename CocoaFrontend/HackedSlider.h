#import <Cocoa/Cocoa.h>

@interface HackedSlider : NSSlider
{
	BOOL duringMouseDown;
}
- (BOOL) mouseTrackingInProgress;
@end
