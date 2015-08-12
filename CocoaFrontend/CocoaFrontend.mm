/*
 *  CocoaFrontend.mm
 *  QuantumGL
 *
 *  Created by wolfgang on Sat Oct 20 2001.
 *
 */

#include <Cocoa/Cocoa.h>
#include "QuantumApplication.h"

int main(int argc, const char **argv)
{
	[QuantumApplication sharedApplication];
	return NSApplicationMain(argc,argv);
}
