//
//  VarWindowController.mm
//  QuantumGL
//
//  Created by wolfgang on Sun Oct 21 2001.
//

#import "VarWindowController.h"
#import "QuantumController.h"
#import "HackedSlider.h"
#import "QGLVariableTableView.h"

#include <algorithm>
#include <string>

using namespace std;

#include "QuantumDescription.h"
#include "RealVariable.h"

@implementation VarWindowController

- init
{
	self = [super init];
	predefinedVariables = [[NSArray alloc] initWithObjects:
		@"latitude", @"longitude", @"distance", @"fovy",
		@"lightLatitude", @"lightLongitude", @"ambient",
		@"lineWidth", nil];
	predefinedVariablesSet = [[NSSet alloc] initWithArray:
		predefinedVariables];
	return self;
}

- (void)awakeFromNib
{
	[[NSNotificationCenter defaultCenter]
		addObserver: self
		selector: @selector(variableChanged:)
		name: @"VariableChanged"
		object: [QuantumController controller]];
	[tableView setBuiltinCount: [predefinedVariables count]];
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver: self];
	[predefinedVariables release];
	[predefinedVariablesSet release];
	[super dealloc];
}

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return definedRealVariables.size();
}

- (RealVariable*) realVariableForRow: (int) rowIndex
{
	definedRealVariablesMap::iterator p;
	//advance(p,rowIndex);

	if(rowIndex < (int)[predefinedVariables count])
	{
		p = definedRealVariables.find(string(
				[[predefinedVariables objectAtIndex: rowIndex] cString]
			));
	}
	else
	{
		rowIndex -= [predefinedVariables count];
		p = definedRealVariables.begin();
		
		for(;;)
		{
			while([predefinedVariablesSet containsObject:
				[NSString stringWithCString: p->first.c_str()]])
				++p;
			if(!rowIndex--)
				break;
			++p;
		}
	}

	return p->second;
}

- (void)updateSlider
{
	int nRows = [tableView numberOfSelectedRows];
	if(nRows == 1)
	{
		int rowIndex = [tableView selectedRow];
		RealVariable *var = [self realVariableForRow: rowIndex];
		if(var->theRange != fullRange)
		{
			[valueSlider setEnabled: YES];
			[valueSlider setMinValue: var->theRange.first];
			[valueSlider setMaxValue: var->theRange.second];
			[valueSlider setDoubleValue: var->value];
		}
		else
			[valueSlider setEnabled: NO];
	}
	else
	{
		[valueSlider setEnabled: NO];
	}
}

- (id)tableView:(NSTableView *)aTableView
		objectValueForTableColumn:(NSTableColumn *)aTableColumn
		row:(int)rowIndex
{
	RealVariable *var = [self realVariableForRow: rowIndex];
	
	if([[aTableColumn identifier] isEqualToString:@"Variable"])
		return [NSString stringWithCString: var->name.c_str()];
	else if([[aTableColumn identifier] isEqualToString:@"Value"])
		return [NSString stringWithFormat: @"%g", var->value];
	else
		return nil;
}

- (void)tableView:(NSTableView *)aTableView
		setObjectValue:(id)str
		forTableColumn:(NSTableColumn *)aTableColumn
		row:(int)rowIndex;
{
	if(![str isKindOfClass: [NSString class]])
		return;
	double d;
	if([[NSScanner scannerWithString: str] scanDouble: &d])
	{
		RealVariable *var = [self realVariableForRow: rowIndex];
		
		if(d != var->value)
		{
			[[QuantumController controller] 
						changeVariableNamed: var->name.c_str()
						toValue: d];
			[self updateSlider];
		}
	}
}


- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	[self updateSlider];
}

- (void)sliderMoved: sender
{
	int nRows = [tableView numberOfSelectedRows];
	if(nRows == 1)
	{
		int rowIndex = [tableView selectedRow];
		RealVariable *var = [self realVariableForRow: rowIndex];
		if(var->theRange != fullRange)
		{
			double d = [valueSlider doubleValue];
			if([valueSlider mouseTrackingInProgress])
				[[QuantumController controller] 
							delayedChangeVariableNamed: var->name.c_str()
							toValue: d];
			else
				[[QuantumController controller] 
							changeVariableNamed: var->name.c_str()
							toValue: d];
		}
	}
	//NSLog(@"%@, %@",[[NSRunLoop currentRunLoop] currentMode],CFRunLoopCopyAllModes([[NSRunLoop currentRunLoop] getCFRunLoop]));

}

- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem
{
	return [[QuantumController controller] isQglOpen];
}
- (IBAction)showVariablesWindow: (id)sender
{
	[tableView reloadData];
	[varWindow orderFront: sender];
}

- (void)variableChanged:(NSNotification *) note
{
	[tableView reloadData];
}

@end
