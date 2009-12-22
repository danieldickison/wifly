//
//  FlipsideViewController.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "FlipsideViewController.h"
#import "iX_YokeAppDelegate.h"
#import "HelpViewController.h"
#import "TrackPadControl.h"
#import "CalibrationViewController.h"


@implementation FlipsideViewController

@synthesize delegate, ipField, portField, tiltView, doneButton, helpButton;


- (void)dealloc
{
    [super dealloc];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationItem.rightBarButtonItem = doneButton;
    self.navigationItem.leftBarButtonItem = helpButton;
    self.title = NSLocalizedString(@"Setup", @"");
    
    ipField.text = SharedAppDelegate.hostAddress;
    portField.text =
        (SharedAppDelegate.hostPort == 0 ? nil
         : [NSString stringWithFormat:@"%d", SharedAppDelegate.hostPort]);

    tiltView.interactionMode = TrackPadTouchesIgnored;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.ipField = nil;
    self.portField = nil;
    self.tiltView = nil;
    self.doneButton = nil;
    self.helpButton = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    SharedAppDelegate.tilt_hold = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:SharedAppDelegate];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)tiltUpdated:(NSNotification *)notification
{
    tiltView.xValue = SharedAppDelegate.tilt_x;
    tiltView.yValue = 1.0f - SharedAppDelegate.tilt_y;
}


- (IBAction)done
{
	[self.delegate flipsideViewControllerDidFinish:self];	
}


- (IBAction)calibrate
{
    CalibrationViewController *calibrationController = [[CalibrationViewController alloc] initWithNibName:@"CalibrationView" bundle:nil];
    [self.navigationController pushViewController:calibrationController animated:YES];
    [calibrationController release];
}


- (IBAction)ipFieldChanged
{
    SharedAppDelegate.hostAddress = ipField.text;
}

- (IBAction)portFieldChanged
{
    SharedAppDelegate.hostPort = [portField.text intValue];
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}


- (IBAction)showHelp
{
    HelpViewController *helpController = [[HelpViewController alloc] init];
    [self.navigationController pushViewController:helpController animated:YES];
    [helpController release];
}


@end
