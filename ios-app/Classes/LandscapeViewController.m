//
//  LandscapeViewController.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "LandscapeViewController.h"
#import "TrackPadControl.h"
#import "iX_YokeAppDelegate.h"
#import "MultiStateButton.h"

enum {
    kAutoHoldTriggerLeft = 0,
    kAutoHoldTriggerBoth,
    kAutoHoldTriggerRight
};

@implementation LandscapeViewController
@synthesize leftTrackPad;
@synthesize rightTrackPad;
@synthesize tiltButton;
@synthesize triggerButtons;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        // Custom initialization
    }
    return self;
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [tiltButton setupForTiltHold];
}

- (void)viewDidUnload
{
    [self setLeftTrackPad:nil];
    [self setRightTrackPad:nil];
    [self setTiltButton:nil];
    [self setTriggerButtons:nil];
    [super viewDidUnload];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    SharedAppDelegate.tiltController.landscape = YES;
    
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    leftTrackPad.autoCenterMode = [prefs integerForKey:@"autoCenterLeft"];
    rightTrackPad.autoCenterMode = [prefs integerForKey:@"autoCenterRight"];
    
    autoHold = [prefs boolForKey:@"autoHold"];
    autoHoldTrigger = [prefs integerForKey:@"autoHoldTrigger"];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (IBAction)infoButtonAction
{
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
    controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
    
    UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
    navController.navigationBar.barStyle = UIBarStyleBlack;
	[self presentModalViewController:navController animated:YES];
    
    [controller release];
	[navController release];
}

- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}

- (IBAction)leftTrackPadChanged
{
    SharedAppDelegate.remoteController.trackpad1_x = leftTrackPad.xValue;
    SharedAppDelegate.remoteController.trackpad1_y = leftTrackPad.yValue;
}

- (IBAction)leftTrackPadTouchDown
{
    touchingLeft = YES;
    [self updateAutoHold];
}

- (IBAction)leftTrackPadTouchUp
{
    touchingLeft = NO;
    [self updateAutoHold];
}

- (IBAction)rightTrackPadChanged
{
    SharedAppDelegate.remoteController.trackpad2_x = rightTrackPad.xValue;
    SharedAppDelegate.remoteController.trackpad2_y = rightTrackPad.yValue;
}

- (IBAction)rightTrackPadTouchDown
{
    touchingRight = YES;
    [self updateAutoHold];
}

- (IBAction)rightTrackPadTouchUp
{
    touchingRight = NO;
    [self updateAutoHold];
}

- (IBAction)tiltHoldAction
{
    TiltController *tilt = SharedAppDelegate.tiltController;
    if (autoHold)
    {
        autoHold = NO;
        tilt.hold = YES;
    }
    else if (tilt.hold)
    {
        autoHold = NO;
        tilt.hold = NO;
    }
    else
    {
        autoHold = YES;
        tilt.hold = YES;
    }
    tiltButton.customStates = self.tiltButtonState;
}

- (UIControlState)tiltButtonState
{
    return ((SharedAppDelegate.tiltController.hold ? kHoldStateOn : 0) |
            (autoHold ? kHoldStateAuto : 0));
}

- (void)updateAutoHold
{
    if (autoHold)
    {
        BOOL tilt = ((touchingLeft && (autoHoldTrigger == kAutoHoldTriggerLeft || autoHoldTrigger == kAutoHoldTriggerBoth)) ||
                     (touchingRight && (autoHoldTrigger == kAutoHoldTriggerRight || autoHoldTrigger == kAutoHoldTriggerBoth)));
        SharedAppDelegate.tiltController.hold = !tilt;
        tiltButton.customStates = self.tiltButtonState;
    }
}

@end
