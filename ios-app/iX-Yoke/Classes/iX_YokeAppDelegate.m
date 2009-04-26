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

#define kUpdateFrequency 15  // Hz
#define kFilteringFactor 0.3f


@implementation iX_YokeAppDelegate


@synthesize window;
@synthesize mainViewController, hostAddress, hostPort;


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
    float pitch = pitchOffset + atan2f(zAvg, sqrtf(ySqr + xAvg*xAvg));
    float roll = rollOffset + atan2f(xAvg, sqrtf(ySqr + zAvg*zAvg));
    
    [mainViewController updatePitch:pitch roll:roll];
    
    if (hostAddress)
    {
        SInt8 bytes[4];
        bytes[0] = 1; //tag for pitch;
        bytes[1] = 180*pitch/M_PI;
        bytes[2] = 2; //tag for roll;
        bytes[3] = 180*roll/M_PI;
        NSData *data = [[NSData alloc] initWithBytes:bytes length:4];
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
