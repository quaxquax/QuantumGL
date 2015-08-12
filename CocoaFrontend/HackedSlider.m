#import "HackedSlider.h"

@implementation HackedSlider
- (void) mouseDown: (NSEvent*) e
{
	duringMouseDown = YES;
	[super mouseDown: e];
	duringMouseDown = NO;
	[self sendAction: [self action] to: [self target]];
}

- (BOOL) mouseTrackingInProgress
{
	return duringMouseDown;
}
@end
