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
    [calibrationTrackPad setMinX:-1 maxX:1 minY:1 maxY:-1];
    calibrationTrackPad.interactionMode = TrackPadTouchesIgnored;
    calibrationTrackPad.clearsContextBeforeDrawing = NO;
    calibrationTrackPad.tag = kTrackPadTag;
    [calibrationTrackPad addTarget:self action:@selector(calibrationUpdated) forControlEvents:UIControlEventValueChanged];
    calibrationTrackPad.multipleTouchEnabled = YES;
    [self.view addSubview:calibrationTrackPad];
    
    controlTrackPad = [[TrackPadControl alloc] initWithFrame:CGRectMake(0, 0, 320, 320)];
    [controlTrackPad setMinX:-1 maxX:1 minY:-1 maxY:1];
    controlTrackPad.interactionMode = TrackPadTouchesValueRelative;
    controlTrackPad.clearsContextBeforeDrawing = NO;
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
	
#ifdef __IPHONE_3_0
#if __IPHONE_OS_VERSION_MIN_REQUIRED > __IPHONE_3_0
	controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
#endif
#endif
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
    calibrationTrackPad.holding = sender.on;
    calibrationTrackPad.interactionMode = (sender.on ? TrackPadTouchesBounds : TrackPadTouchesIgnored);
    controlTrackPad.holding = sender.on;
    controlTrackPad.interactionMode = (sender.on ? TrackPadTouchesBounds : TrackPadTouchesValueRelative);
}

- (IBAction)switchTrackPad:(UISegmentedControl *)sender
{
    [[self.view viewWithTag:kTrackPadTag] removeFromSuperview];
    [self.view addSubview:(sender.selectedSegmentIndex == 0
                           ? calibrationTrackPad
                           : controlTrackPad)];
}


- (IBAction)resetCenter
{
    [SharedAppDelegate resetTiltCenter];
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
