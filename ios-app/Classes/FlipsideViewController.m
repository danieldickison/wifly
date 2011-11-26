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
#import "BonjourViewController.h"
#import "MultiStateButton.h"


@implementation FlipsideViewController

@synthesize autoCenterLeft;
@synthesize autoCenterRight;
@synthesize autoHoldTrigger;

@synthesize delegate, ipField, portField, doneButton, helpButton;


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationItem.rightBarButtonItem = doneButton;
    self.navigationItem.leftBarButtonItem = helpButton;
    self.title = NSLocalizedString(@"Setup", @"");

    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    
    [autoCenterLeft setupForTrackPadAutoCenter];
    autoCenterLeft.customStates = [prefs integerForKey:@"autoCenterLeft"];
    
    [autoCenterRight setupForTrackPadAutoCenter];
    autoCenterRight.customStates = [prefs integerForKey:@"autoCenterRight"];
    
    autoHoldTrigger.selectedSegmentIndex = [prefs integerForKey:@"autoHoldTrigger"];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    [self setAutoCenterRight:nil];
    [self setAutoCenterLeft:nil];
    [self setAutoHoldTrigger:nil];
    self.ipField = nil;
    self.portField = nil;
    self.doneButton = nil;
    self.helpButton = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    RemoteController *remote = SharedAppDelegate.remoteController;
    ipField.text = remote.hostAddress;
    portField.text =
    (remote.hostPort == 0 ? nil
     : [NSString stringWithFormat:@"%d", remote.hostPort]);
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
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
    SharedAppDelegate.remoteController.hostAddress = ipField.text;
}

- (IBAction)portFieldChanged
{
    SharedAppDelegate.remoteController.hostPort = [portField.text intValue];
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

- (IBAction)showBonjour
{
    BonjourViewController *c = [[BonjourViewController alloc] initWithNibName:@"BonjourViewController" bundle:nil];
    [self.navigationController pushViewController:c animated:YES];
    [c release];
}

- (IBAction)autoHoldTriggerChanged
{
    [[NSUserDefaults standardUserDefaults] setInteger:autoHoldTrigger.selectedSegmentIndex forKey:@"autoHoldTrigger"];
}

- (IBAction)autoCenterLeftChanged
{
    [[NSUserDefaults standardUserDefaults] setInteger:autoCenterLeft.customStates forKey:@"autoCenterLeft"];
}

- (IBAction)autoCenterRightChanged
{
    [[NSUserDefaults standardUserDefaults] setInteger:autoCenterRight.customStates forKey:@"autoCenterRight"];
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
}


@end
