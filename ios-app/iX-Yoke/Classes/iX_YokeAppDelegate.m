//
//  iX_YokeAppDelegate.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "iX_YokeAppDelegate.h"
#import "MainViewController.h"
#import "AsyncUdpSocket.h"
#include "iX_Yoke_Network.h"

#define kUpdateFrequency 20  // Hz
#define kFilteringFactor 0.25f


@implementation iX_YokeAppDelegate


@synthesize window;
@synthesize mainViewController, hostAddress, hostPort, touch_x, touch_y, suspended;


- (void)setHostAddress:(NSString *)addr
{
    if (addr != hostAddress)
    {
        [hostAddress release];
        hostAddress = [addr retain];
        [[NSUserDefaults standardUserDefaults] setObject:addr forKey:@"hostAddress"];
    }
}

- (void)setHostPort:(unsigned)port
{
    hostPort = port;
    [[NSUserDefaults standardUserDefaults] setInteger:port forKey:@"hostPort"];
}


- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    socket = [[AsyncUdpSocket alloc] initWithDelegate:self];
    
    self.hostAddress = [[NSUserDefaults standardUserDefaults] stringForKey:@"hostAddress"];
    self.hostPort = [[NSUserDefaults standardUserDefaults] integerForKey:@"hostPort"];
    
    [UIAccelerometer sharedAccelerometer].updateInterval = (1.0 / kUpdateFrequency);
    [UIAccelerometer sharedAccelerometer].delegate = self;
    
	MainViewController *aController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
	self.mainViewController = aController;
	[aController release];
	
    mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	[window addSubview:[mainViewController view]];
    [window makeKeyAndVisible];
}


- (void)dealloc
{
    [mainViewController release];
    [window release];
    [super dealloc];
}


- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
    // Use a basic low-pass filter to only keep the gravity in the accelerometer values for the X and Y axes
    // See the BubbleLevel Apple example.
    xAvg = (float)acceleration.x * kFilteringFactor + xAvg * (1.0f - kFilteringFactor);
    yAvg = (float)acceleration.y * kFilteringFactor + yAvg * (1.0f - kFilteringFactor);
    zAvg = (float)acceleration.z * kFilteringFactor + zAvg * (1.0f - kFilteringFactor);
    
    // This method gives a range of +/- 90 degrees on each axis, but only if you're rotating about one axis at a time.  If you try to tilt on both axes, then it maxes out at +/- 45 degrees.  It would probably be nicer if the range is kept constant regardless of whether you're tilting in one or two axes.
    float ySqr = yAvg*yAvg;
    float pitchRad = pitchOffset + atan2f(zAvg, sqrtf(ySqr + xAvg*xAvg));
    float rollRad = rollOffset + atan2f(xAvg, sqrtf(ySqr + zAvg*zAvg));
    float pitch = pitchRad / M_PI_2;
    float roll = rollRad / M_PI_2;
    
    // The view controller handles calibration via its trackpad.
    [mainViewController updatePitch:&pitch roll:&roll];
    
    if (hostAddress && !suspended)
    {
        UInt8 buffer[128];
        int i = 0;
        ix_put_tag(buffer, &i, kProtocolVersion1Tag);
        ix_put_ratio(buffer, &i, roll);
        ix_put_ratio(buffer, &i, pitch);
        ix_put_ratio(buffer, &i, touch_x);
        ix_put_ratio(buffer, &i, touch_y);
        NSData *data = [[NSData alloc] initWithBytes:buffer length:i];
        [socket sendData:data toHost:hostAddress port:hostPort withTimeout:-1 tag:0];
        [data release];
    }
}


- (void)resetCalibration
{
    float ySqr = yAvg*yAvg;
    pitchOffset = -atan2f(zAvg, sqrtf(ySqr + xAvg*xAvg));
    rollOffset = -atan2f(xAvg, sqrtf(ySqr + zAvg*zAvg));
}


@end
