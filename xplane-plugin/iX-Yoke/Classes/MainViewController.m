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


@implementation MainViewController

@synthesize rollIndicator, pitchIndicator, rollLabel, pitchLabel;

-(void)dealloc
{
    self.rollIndicator = nil;
    self.pitchIndicator = nil;
    self.rollLabel = nil;
    self.pitchLabel = nil;
    [super dealloc];
}


 - (void)viewDidLoad
{
    [super viewDidLoad];
    pitchIndicator.transform = CGAffineTransformMakeRotation(-M_PI_2);
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


- (void)updatePitch:(float)pitch roll:(float)roll
{
    pitchIndicator.progress = (pitch + M_PI_2) / M_PI;
    rollIndicator.progress = (roll + M_PI_2) / M_PI;
    pitchLabel.text = [NSString stringWithFormat:@"%d%C", (int)(180*pitch/M_PI), 0xB0];
    rollLabel.text = [NSString stringWithFormat:@"%d%C", (int)(180*roll/M_PI), 0xB0];
}


- (IBAction)resetCalibration
{
    [SharedAppDelegate resetCalibration];
}


/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */




@end
