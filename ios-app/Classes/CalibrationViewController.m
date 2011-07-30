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

#import "CalibrationViewController.h"
#import "TrackPadControl.h"
#import "iX_YokeAppDelegate.h"


enum {
    kStepOrientCenter = 100,
    kStepTapCenter,
    kStepOrientForward,
    kStepTapForward,
    kStepDone
};


@interface CalibrationViewController ()

- (void)updateInstructions;

@end


@implementation CalibrationViewController

@synthesize doneButton, cancelButton, tiltView;

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.title = NSLocalizedString(@"Calibration", @"");
    self.navigationItem.rightBarButtonItem = doneButton;
    self.navigationItem.leftBarButtonItem = cancelButton;
}

- (void)viewDidUnload
{
    self.tiltView = nil;
    self.doneButton = nil;
    self.cancelButton = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    SharedAppDelegate.tiltController.hold = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:nil];
    currentStep = kStepOrientCenter;
    [self updateInstructions];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)tiltUpdated:(NSNotification *)notification
{
    tiltView.xValue = SharedAppDelegate.tiltController.x;
    tiltView.yValue = 1.0f - SharedAppDelegate.tiltController.y;
}



- (IBAction)done
{
    [SharedAppDelegate.tiltController setPortraitCalibrationWithCenterVector:centerVector forwardVector:forwardVector];
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)cancel
{
    [self.navigationController popViewControllerAnimated:YES];
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (currentStep != kStepDone) currentStep++;
    [self updateInstructions];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    currentStep--;
    [self updateInstructions];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (currentStep == kStepTapCenter)
    {
        [SharedAppDelegate.tiltController getAccelerationVector:centerVector];
    }
    else if (currentStep == kStepTapForward)
    {
        [SharedAppDelegate.tiltController getAccelerationVector:forwardVector];
    }
    
    if (currentStep != kStepDone) currentStep++;
    [self updateInstructions];
}


- (void)updateInstructions
{
    for (int i = kStepOrientCenter; i <= kStepDone; i++)
    {
        UIColor *color;
        if (i < currentStep)
            color = [UIColor colorWithRed:0 green:0.75 blue:0 alpha:1.0];
        else if (i > currentStep)
            color = [UIColor grayColor];
        else
            color = [UIColor whiteColor];
        
        UILabel *label = (UILabel *)[self.view viewWithTag:i];
        label.textColor = color;
    }
}

@end
