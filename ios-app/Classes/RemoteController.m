//
//  RemoteController.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RemoteController.h"
#import "iX_Yoke_Network.h"
#import "AsyncUdpSocket.h"

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
        socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
        
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

- (void)send
{
    if (hostAddress)
    {
        uint8_t buffer[128];
        int i = 0;
        ix_put_tag(buffer, &i, kProtocolVersion1Tag);
        ix_put_ratio(buffer, &i, tilt_x);
        ix_put_ratio(buffer, &i, tilt_y);
        ix_put_ratio(buffer, &i, trackpad1_x);
        ix_put_ratio(buffer, &i, trackpad1_y);
        ix_put_ratio(buffer, &i, trackpad2_x);
        ix_put_ratio(buffer, &i, trackpad2_y);
        NSData *data = [[NSData alloc] initWithBytes:buffer length:i];
        [socket sendData:data toHost:hostAddress port:hostPort withTimeout:-1 tag:0];
        [data release];
    }
}

- (void)setHostAddress:(NSString *)address
{
    [hostAddress autorelease];
    hostAddress = [address copy];
    [[NSUserDefaults standardUserDefaults] setObject:hostAddress forKey:@"hostAddress"];
}

- (void)setHostPort:(unsigned int)port
{
    hostPort = port;
    [[NSUserDefaults standardUserDefaults] setInteger:hostPort forKey:@"hostPort"];
}

@end
