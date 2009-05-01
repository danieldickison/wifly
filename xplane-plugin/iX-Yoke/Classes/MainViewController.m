//
//  MainViewController.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "MainViewController.h"
#import "MainView.h"
#import "iX_YokeAppDelegate.h"
#import "TrackPadControl.h"


#define kTrackPadTag 562


@implementation MainViewController

-(void)dealloc
{
    [calibrationTrackPad release];
    [controlTrackPad release];
    [super dealloc];
}


 - (void)viewDidLoad
{
    [super viewDidLoad];
    
    calibrationTrackPad = [[TrackPadControl alloc] initWithFrame:CGRectMake(0, 0, 320, 320)];
    calibrationTrackPad.trackingBounds = CGRectMake(30, 10, 240, 180);
    [calibrationTrackPad setMinX:-1 maxX:1 minY:1 maxY:-1];
    calibrationTrackPad.singleTouchCenters = YES;
    calibrationTrackPad.tag = kTrackPadTag;
    [calibrationTrackPad addTarget:self action:@selector(calibrationUpdated) forControlEvents:UIControlEventValueChanged];
    calibrationTrackPad.multipleTouchEnabled = YES;
    [self.view addSubview:calibrationTrackPad];
    
    controlTrackPad = [[TrackPadControl alloc] initWithFrame:CGRectMake(0, 0, 320, 320)];
    [controlTrackPad setMinX:-1 maxX:1 minY:-1 maxY:1];
    controlTrackPad.xValue = SharedAppDelegate.touch_x;
    controlTrackPad.yValue = SharedAppDelegate.touch_y;
    controlTrackPad.tag = kTrackPadTag;
    [controlTrackPad addTarget:self action:@selector(controlUpdated) forControlEvents:UIControlEventValueChanged];
    controlTrackPad.multipleTouchEnabled = YES;
}


- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}


- (IBAction)showInfo {    
	
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
	
	controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
	[self presentModalViewController:controller animated:YES];
	
	[controller release];
}


- (void)updatePitch:(float *)ioPitch roll:(float *)ioRoll
{
    // Use the trackpad to calibrate pitch and roll.
    calibrationTrackPad.xValue = *ioRoll;
    calibrationTrackPad.yValue = *ioPitch;
    *ioRoll = calibrationTrackPad.xValue;
    *ioPitch = calibrationTrackPad.yValue;
}


- (IBAction)toggleSuspend:(UISwitch *)sender
{
    SharedAppDelegate.suspended = sender.on;
}

- (IBAction)switchTrackPad:(UISegmentedControl *)sender
{
    [[self.view viewWithTag:kTrackPadTag] removeFromSuperview];
    [self.view addSubview:(sender.selectedSegmentIndex == 0
                           ? calibrationTrackPad
                           : controlTrackPad)];
}


- (void)calibrationUpdated
{
}


- (void)controlUpdated
{
    SharedAppDelegate.touch_x = controlTrackPad.xValue;
    SharedAppDelegate.touch_y = controlTrackPad.yValue;
}



@end
