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


@implementation FlipsideViewController

@synthesize delegate, ipField, portField, tiltView, doneButton, helpButton;


- (void)dealloc
{
    [super dealloc];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationItem.leftBarButtonItem = doneButton;
    self.navigationItem.rightBarButtonItem = helpButton;
    self.title = NSLocalizedString(@"Setup", @"");
    
    ipField.text = SharedAppDelegate.hostAddress;
    portField.text =
        (SharedAppDelegate.hostPort == 0 ? nil
         : [NSString stringWithFormat:@"%d", SharedAppDelegate.hostPort]);

    tiltView.interactionMode = TrackPadTouchesIgnored;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:SharedAppDelegate];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.ipField = nil;
    self.portField = nil;
    self.tiltView = nil;
    self.doneButton = nil;
    self.helpButton = nil;
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


- (IBAction)setTiltCenter
{
    [SharedAppDelegate resetTiltCenter];
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
