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

@synthesize trackpad, tiltView;

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
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.trackpad = nil;
    self.tiltView = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)tiltUpdated:(NSNotification *)notification
{
    [tiltView setXValue:SharedAppDelegate.tilt_hold_x yValue:SharedAppDelegate.tilt_hold_y xCrosshair:SharedAppDelegate.tilt_x yCrosshair:SharedAppDelegate.tilt_y];
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


- (IBAction)tiltHold:(UISwitch *)holdSwitch
{
    SharedAppDelegate.tilt_hold = holdSwitch.on;
}



@end
