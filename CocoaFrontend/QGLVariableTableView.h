/* QGLVariableTableView */

#import <Cocoa/Cocoa.h>

@interface QGLVariableTableView : NSTableView
{
	int builtinCount;
}
- (void) setBuiltinCount: (int) count;
- (IBAction)copy:(id)sender;
@end
