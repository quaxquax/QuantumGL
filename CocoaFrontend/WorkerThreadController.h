//
//  WorkerThreadController.h
//  QuantumGL
//
//  Created by wolfgang on Sun Oct 21 2001.
//  Copyright (c) 2001 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@protocol UIThreadMethods;
@class QuantumController;

@protocol WorkerThreadMethods
- (oneway void) loadDescriptionFile: (NSString *)file;
//- (oneway void) releaseDescription;
- (oneway void) quantumUpdate;
- (void) releaseDescription;	// important: mustn't be oneway
@end

@interface WorkerThreadController : NSObject <WorkerThreadMethods> {
	id<UIThreadMethods>	uiThreadProxy;
	QuantumController	*qController;
}
+ (void)workerThreadMain: (NSArray*) portsAndController;

- initWithController: contr andProxy: proxy;

- (void) loadDescriptionFile: (NSString *)file;
- (void) quantumUpdate;

- (void) releaseDescription;
@end
