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

@implementation LandscapeViewController
@synthesize leftTrackPad;
@synthesize rightTrackPad;

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
    // Do any additional setup after loading the view from its nib.
}

- (void)viewDidUnload
{
    [self setLeftTrackPad:nil];
    [self setRightTrackPad:nil];
    [super viewDidUnload];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    SharedAppDelegate.tiltController.landscape = YES;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void)dealloc {
    [leftTrackPad release];
    [rightTrackPad release];
    [super dealloc];
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

- (IBAction)leftTrackPadTouchDown {
}

- (IBAction)leftTrackPadTouchUp {
}

- (IBAction)rightTrackPadChanged
{
    SharedAppDelegate.remoteController.trackpad2_x = rightTrackPad.xValue;
    SharedAppDelegate.remoteController.trackpad2_y = rightTrackPad.yValue;
}

- (IBAction)rightTrackPadTouchDown {
}

- (IBAction)rightTrackPadTouchUp {
}

@end
