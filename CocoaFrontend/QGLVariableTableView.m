#import "QGLVariableTableView.h"

@implementation QGLVariableTableView

- (IBAction)copy:(id)sender
{
	NSPasteboard *pb = [NSPasteboard generalPasteboard];
	NSMutableArray *lines = [[NSMutableArray alloc] init] ;
	
	NSTableColumn *col1 = [self tableColumnWithIdentifier: @"Variable"];
	NSTableColumn *col2 = [self tableColumnWithIdentifier: @"Value"];
	id dataSource = [self dataSource];
	NSEnumerator *rows = [self selectedRowEnumerator];
	NSNumber *row;
	
	NSAssert(col1 != nil, @"Need column \"Variable\"");
	NSAssert(col2 != nil, @"Need column \"Value\"");
	
	while((row = [rows nextObject]) != nil)
	{
		[lines addObject: [NSString stringWithFormat: @"%@ := %@;",
			[dataSource tableView: self objectValueForTableColumn:col1 row: [row intValue]],
			[dataSource tableView: self objectValueForTableColumn:col2 row: [row intValue]]]];
	}
	
	[pb declareTypes: [NSArray arrayWithObject: NSStringPboardType] owner: nil];
	[pb setString: [lines componentsJoinedByString: @"\n"] forType: NSStringPboardType];
	
	[lines release]; 
}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	if([NSStringFromSelector([menuItem action]) isEqualToString: @"copy:"])
	{
		return [self numberOfSelectedRows] >= 1;
	}
	else
	{
		if([[self superclass] instancesRespondToSelector: @selector(validateMenuItem:)])
			return [super validateMenuItem: menuItem];
		else
			return YES;
	}
}

- (void) setBuiltinCount: (int) count
{
	builtinCount = count;
}

- (void)highlightSelectionInClipRect:(NSRect)clipRect
{
	int row;
	NSRect rect;
	[[NSColor colorWithDeviceWhite: 0.95 alpha: 1.0] set];
	for(row = 0; row < builtinCount; row++)
	{
		rect = [self rectOfRow: row];
		NSRectFill(rect);
	}
	[[NSColor colorWithDeviceWhite: 0.25 alpha: 1.0] set];
	rect.origin.y += rect.size.height;//-1;
	rect.size.height = 1;
	NSRectFill(rect);
	
	[super highlightSelectionInClipRect: clipRect];
}

@end
