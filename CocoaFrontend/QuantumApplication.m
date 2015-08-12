//
//  QuantumApplication.m
//  QuantumGL
//
//  Created by Wolfgang Thaller on Sun Sep 14 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//

#import "QuantumApplication.h"


@implementation QuantumApplication

- init
{
	self = [super init];
	NSLog(@"app loaded");
	return self;
}

- (void) showHelp: (id) sender
{
	[[NSWorkspace sharedWorkspace] openFile: 
		[[NSBundle mainBundle]
			pathForResource:@"QuantumGL Documentation"
			ofType:@"pdf"]];
}

- (void)orderFrontStandardAboutPanelWithOptions:
			(NSDictionary *)optionsDictionary
{
	//[[NSWindowController alloc] initWithWindowNibName: @"AboutBox"];
	//[NSBundle loadNibNamed: @"AboutBox" owner: self];
	[aboutBox makeKeyAndOrderFront: self];
}
@end
