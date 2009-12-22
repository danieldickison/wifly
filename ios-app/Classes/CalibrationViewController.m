//
//  CalibrationViewController.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 12/22/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

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
    SharedAppDelegate.tilt_hold = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:SharedAppDelegate];
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
    tiltView.xValue = SharedAppDelegate.tilt_x;
    tiltView.yValue = 1.0f - SharedAppDelegate.tilt_y;
}



- (IBAction)done
{
    //[SharedAppDelegate setCalibrationCenter...];
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
