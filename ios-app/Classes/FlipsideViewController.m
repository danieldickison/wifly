//Copyright (c) 2011 Daniel Dickison
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

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
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:SharedAppDelegate];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)tiltUpdated:(NSNotification *)notification
{
    [tiltView setXValue:SharedAppDelegate.tilt_hold_x yValue:(1.0f - SharedAppDelegate.tilt_hold_y) xCrosshair:SharedAppDelegate.tilt_x yCrosshair:(1.0f - SharedAppDelegate.tilt_y)];
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
