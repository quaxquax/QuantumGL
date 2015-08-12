//
//  VarWindowController.h
//  QuantumGL
//
//  Created by wolfgang on Sun Oct 21 2001.
//  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

class RealVariable;

@class QGLVariableTableView;

@interface VarWindowController : NSObject {
	IBOutlet NSPanel*		varWindow;
	IBOutlet QGLVariableTableView*	tableView;
	IBOutlet id				valueSlider;

	RealVariable	*curVar;
	NSArray			*predefinedVariables;
	NSSet			*predefinedVariablesSet;
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView;

- (id)tableView:(NSTableView *)aTableView
		objectValueForTableColumn:(NSTableColumn *)aTableColumn
		row:(int)rowIndex;

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
/*- (void)tableView:(NSTableView *)aTableView
		setObjectValue:(id)anObject
		forTableColumn:(NSTableColumn *)aTableColumn
		row:(int)rowIndex;*/
		
- (void)sliderMoved: sender;

- (IBAction)showVariablesWindow: (id)sender;
@end
