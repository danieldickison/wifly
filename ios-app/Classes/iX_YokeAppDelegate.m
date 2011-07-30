//Copyright (c) 2011 Daniel Dickison
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#import "iX_YokeAppDelegate.h"
#import "MainViewController.h"
#import "LandscapeViewController.h"
#import "AsyncUdpSocket.h"
#include "iX_Yoke_Network.h"


@implementation iX_YokeAppDelegate

@synthesize window;
@synthesize mainViewController;
@synthesize remoteController;
@synthesize tiltController;


- (void)applicationWillTerminate:(UIApplication *)application
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults synchronize];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [self applicationWillTerminate:application];
}


- (void)applicationDidFinishLaunching:(UIApplication *)application
{
    
    remoteController = [[RemoteController alloc] init];
    tiltController = [[TiltController alloc] init];
    
	MainViewController *aController = [[MainViewController alloc] initWithNibName:@"MainView" bundle:nil];
	self.mainViewController = aController;
	[aController release];
    
    landscapeViewController = [[LandscapeViewController alloc] initWithNibName:@"LandscapeViewController" bundle:nil];
	
    mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
    landscapeViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	[window addSubview:[landscapeViewController view]];
    [window makeKeyAndVisible];
    
    if ([remoteController.hostAddress length] == 0)
    {
        [mainViewController performSelector:@selector(showInfo) withObject:nil afterDelay:0.5];
    }
    
    // Prevent sleep so we don't suddenly lose accelerometer readings when there are no touches for a while.
    application.idleTimerDisabled = YES;
}

@end
