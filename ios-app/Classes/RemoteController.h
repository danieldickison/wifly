//
//  RemoteController.h
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@class AsyncUdpSocket;
@class OSCConnection;

@interface RemoteController : NSObject
{
    NSString *hostAddress;
    unsigned hostPort;
    OSCConnection *oscConnection;
    
    float tilt_x;
    float tilt_y;
    float trackpad1_x;
    float trackpad1_y;
    float trackpad2_x;
    float trackpad2_y;
}

@property (nonatomic, copy) NSString *hostAddress;
@property (nonatomic, assign) unsigned hostPort;
- (void)send;

@property (nonatomic, assign) float tilt_x;
@property (nonatomic, assign) float tilt_y;
@property (nonatomic, assign) float trackpad1_x;
@property (nonatomic, assign) float trackpad1_y;
@property (nonatomic, assign) float trackpad2_x;
@property (nonatomic, assign) float trackpad2_y;

@end
