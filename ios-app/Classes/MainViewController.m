//
//  MainViewController.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "MainViewController.h"
#import "iX_YokeAppDelegate.h"
#import "TrackPadControl.h"


#define kTrackPadTag 562


@implementation MainViewController

@synthesize trackpad, tiltView, holdSwitch, autoHoldSwitch;

-(void)dealloc
{
    [super dealloc];
}


 - (void)viewDidLoad
{
    [super viewDidLoad];
    
    trackpad.interactionMode = TrackPadTouchesValueRelative;
    trackpad.xValue = SharedAppDelegate.touch_x;
    trackpad.yValue = SharedAppDelegate.touch_y;
    
    tiltView.interactionMode = TrackPadTouchesIgnored;
    tiltView.pointRadius = 5.0f;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:SharedAppDelegate];
    
    holdSwitch.on = SharedAppDelegate.tilt_hold;
    autoHoldSwitch.on = SharedAppDelegate.auto_hold;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.trackpad = nil;
    self.tiltView = nil;
    self.holdSwitch = nil;
    self.autoHoldSwitch = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)tiltUpdated:(NSNotification *)notification
{
    [tiltView setXValue:SharedAppDelegate.tilt_hold_x yValue:(1.0f - SharedAppDelegate.tilt_hold_y) xCrosshair:SharedAppDelegate.tilt_x yCrosshair:(1.0f - SharedAppDelegate.tilt_y)];
}


- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}


- (IBAction)showInfo {    
	
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
    controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
    
    UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
    navController.navigationBar.barStyle = UIBarStyleBlack;
	[self presentModalViewController:navController animated:YES];
    
    [controller release];
	[navController release];
}



- (IBAction)trackpadUpdated
{
    SharedAppDelegate.touch_x = trackpad.xValue;
    SharedAppDelegate.touch_y = trackpad.yValue;
}


- (IBAction)trackpadTouchDown
{
    if (trackpadTouchCount == 0 &&
        SharedAppDelegate.auto_hold)
    {
        SharedAppDelegate.tilt_hold = NO;
        [holdSwitch setOn:NO animated:YES];
    }
    trackpadTouchCount++;
}

- (IBAction)trackpadTouchUp
{
    trackpadTouchCount--;
    if (trackpadTouchCount == 0 &&
        SharedAppDelegate.auto_hold)
    {
        SharedAppDelegate.tilt_hold = YES;
        [holdSwitch setOn:YES animated:YES];
    }
}



- (IBAction)tiltHold
{
    SharedAppDelegate.tilt_hold = holdSwitch.on;
    SharedAppDelegate.auto_hold = NO;
    [autoHoldSwitch setOn:NO animated:YES];
}

- (IBAction)autoHold
{
    SharedAppDelegate.auto_hold = autoHoldSwitch.on;
    SharedAppDelegate.tilt_hold = autoHoldSwitch.on;
    [holdSwitch setOn:autoHoldSwitch.on animated:YES];
}


@end
