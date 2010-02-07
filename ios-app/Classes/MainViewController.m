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
#import "MultiStateButton.h"


enum {
    kHoldStateAuto = 1 << 16, // First bit of UIControlStateApplication
    kHoldStateOn   = 1 << 17
};

enum {
    kAutoCenterStateX = 1 << 16, // First bit of UIControlStateApplication
    kAutoCenterStateY = 1 << 17
};


@interface MainViewController ()

@property (nonatomic, readonly) UIControlState holdButtonState;
@property (nonatomic, readonly) UIControlState autoCenterButtonState;

@end



@implementation MainViewController

@synthesize trackpad, tiltView, holdButton, autoCenterButton;

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
    
    [holdButton setImage:[UIImage imageNamed:@"Toggle Button Tilt On.png"] forState:kHoldStateOn];
    [holdButton setImage:[UIImage imageNamed:@"Toggle Button Tilt Auto.png"] forState:kHoldStateAuto];
    [holdButton setImage:[UIImage imageNamed:@"Toggle Button Tilt Auto On.png"] forState:kHoldStateOn | kHoldStateAuto];
    holdButton.customStates = self.holdButtonState;
    
    [autoCenterButton setImage:[UIImage imageNamed:@"Toggle Button Center X.png"] forState:kAutoCenterStateX];
    [autoCenterButton setImage:[UIImage imageNamed:@"Toggle Button Center Y.png"] forState:kAutoCenterStateY];
    [autoCenterButton setImage:[UIImage imageNamed:@"Toggle Button Center X Y.png"] forState:kAutoCenterStateX | kAutoCenterStateY];
    autoCenterButton.customStates = self.autoCenterButtonState;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.trackpad = nil;
    self.tiltView = nil;
    self.holdButton = nil;
    self.autoCenterButton = nil;
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
        holdButton.customStates = self.holdButtonState;
    }
    trackpadTouchCount++;
}

- (IBAction)trackpadTouchUp
{
    trackpadTouchCount--;
    if (trackpadTouchCount == 0)
    {
        if (SharedAppDelegate.auto_hold)
        {
            SharedAppDelegate.tilt_hold = YES;
            holdButton.customStates = self.holdButtonState;
        }
        if (SharedAppDelegate.auto_center_x)
        {
            trackpad.xValue = SharedAppDelegate.touch_x = 0.5f;
        }
        if (SharedAppDelegate.auto_center_y)
        {
            trackpad.yValue = SharedAppDelegate.touch_y = 0.5f;
        }
    }
}



- (IBAction)holdAction
{
    if (SharedAppDelegate.auto_hold)
    {
        SharedAppDelegate.auto_hold = NO;
        SharedAppDelegate.tilt_hold = YES;
    }
    else if (SharedAppDelegate.tilt_hold)
    {
        SharedAppDelegate.auto_hold = NO;
        SharedAppDelegate.tilt_hold = NO;
    }
    else
    {
        SharedAppDelegate.auto_hold = YES;
        SharedAppDelegate.tilt_hold = YES;
    }
    holdButton.customStates = self.holdButtonState;
}

- (IBAction)autoCenterAction
{
    if (SharedAppDelegate.auto_center_x && SharedAppDelegate.auto_center_y)
    {
        SharedAppDelegate.auto_center_x = NO;
        SharedAppDelegate.auto_center_y = NO;
    }
    else if (!SharedAppDelegate.auto_center_x)
    {
        SharedAppDelegate.auto_center_x = YES;
        trackpad.xValue = SharedAppDelegate.touch_x = 0.5f;
    }
    else
    {
        SharedAppDelegate.auto_center_x = NO;
        SharedAppDelegate.auto_center_y = YES;
        trackpad.yValue = SharedAppDelegate.touch_y = 0.5f;
    }
    autoCenterButton.customStates = self.autoCenterButtonState;
}


- (UIControlState)holdButtonState
{
    return ((SharedAppDelegate.tilt_hold ? kHoldStateOn : 0) |
            (SharedAppDelegate.auto_hold ? kHoldStateAuto : 0));
}

- (UIControlState)autoCenterButtonState
{
    return ((SharedAppDelegate.auto_center_x ? kAutoCenterStateX : 0) |
            (SharedAppDelegate.auto_center_y ? kAutoCenterStateY : 0));
}


@end
