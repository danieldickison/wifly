//
//  RemoteController.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RemoteController.h"
#import "iX_Yoke_Network.h"
#import <CocoaOSC/CocoaOSC.h>

@implementation RemoteController

@synthesize hostAddress;
@synthesize hostPort;
@synthesize tilt_x, tilt_y;
@synthesize trackpad1_x, trackpad1_y;
@synthesize trackpad2_x, trackpad2_y;

- (id)init
{
    if ((self = [super init]))
    {
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        hostAddress = [defaults stringForKey:@"hostAddress"];
        hostPort = [defaults integerForKey:@"hostPort"];
        if (!self.hostPort)
            self.hostPort = kServerPort;
        
        tilt_x = [defaults floatForKey:@"tilt_x"];
        tilt_y = [defaults floatForKey:@"tilt_y"];
        trackpad1_x = [defaults floatForKey:@"trackpad1_x"];
        trackpad1_y = [defaults floatForKey:@"trackpad1_y"];
        trackpad2_x = [defaults floatForKey:@"trackpad2_x"];
        trackpad2_y = [defaults floatForKey:@"trackpad2_y"];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(terminationNotification:) name:UIApplicationWillTerminateNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(terminationNotification:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    }
    return self;
}

- (void)terminationNotification:(NSNotification *)notification
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setFloat:tilt_x forKey:@"tilt_x"];
    [defaults setFloat:tilt_y forKey:@"tilt_y"];
    [defaults setFloat:trackpad1_x forKey:@"trackpad1_x"];
    [defaults setFloat:trackpad1_y forKey:@"trackpad1_y"];
    [defaults setFloat:trackpad2_x forKey:@"trackpad2_x"];
    [defaults setFloat:trackpad2_y forKey:@"trackpad2_y"];
    [defaults synchronize];
}

- (void)setButton:(NSUInteger)button touching:(BOOL)touching
{
    NSAssert(button >= 1 && button <= kRemoteButtonCount, @"Remote button %u out of range [1..%u]", button, kRemoteButtonCount);
    buttonTouchStates[button - 1] = touching;
}

- (void)send
{
    if (!hostAddress || !hostPort) {
        NSLog(@"Cannot send packet unless address and port are specified!");
        return;
    }
    if (!oscConnection) {
        oscConnection = [[OSCConnection alloc] init];
        NSError *error;
        if (![oscConnection connectToHost:hostAddress port:hostPort protocol:OSCConnectionUDP error:&error]) {
            NSLog(@"Error connecting: %@", error);
            [oscConnection release];
            oscConnection = nil;
            return;
        }
    }
    
    OSCMutableBundle *bundle = [[OSCMutableBundle alloc] init];
    float values[] = {tilt_x, tilt_y, trackpad1_x, trackpad1_y, trackpad2_x, trackpad2_y};
    NSString *addresses[] = {@"/tilt/x", @"/tilt/y", @"/trackpad/1/x", @"/trackpad/1/y", @"/trackpad/2/x", @"/trackpad/2/y"};
    for (int i = 0; i < 6; i++) {
        OSCMutableMessage *message = [[OSCMutableMessage alloc] init];
        message.address = addresses[i];
        [message addFloat:values[i]];
        [bundle addChildPacket:message];
        [message release];
    }
    
    for (int i = 1; i <= kRemoteButtonCount; i++) {
        OSCMutableMessage *message = [[OSCMutableMessage alloc] init];
        message.address = [NSString stringWithFormat:@"/button/%u/touch", i];
        [message addInt:buttonTouchStates[i - 1] ? 1 : 0];
        [bundle addChildPacket:message];
        [message release];
    }
    
    [oscConnection sendPacket:bundle];
    [bundle release];
}

- (void)setHostAddress:(NSString *)address
{
    [hostAddress autorelease];
    hostAddress = [address copy];
    [[NSUserDefaults standardUserDefaults] setObject:hostAddress forKey:@"hostAddress"];
    [oscConnection release];
    oscConnection = nil;
}

- (void)setHostPort:(unsigned int)port
{
    hostPort = port;
    [[NSUserDefaults standardUserDefaults] setInteger:hostPort forKey:@"hostPort"];
    [oscConnection release];
    oscConnection = nil;
}

@end
