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

@synthesize trackpad;

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
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.trackpad = nil;
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


- (IBAction)toggleSuspend:(UISwitch *)sender
{
    trackpad.holding = sender.on;
}


- (IBAction)trackpadUpdated
{
    SharedAppDelegate.touch_x = trackpad.xValue;
    SharedAppDelegate.touch_y = trackpad.yValue;
}



@end
