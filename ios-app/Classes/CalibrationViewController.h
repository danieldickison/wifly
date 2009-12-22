//
//  CalibrationViewController.h
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 12/22/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TrackPadControl;

@interface CalibrationViewController : UIViewController
{
    TrackPadControl *tiltView;
    UIBarButtonItem *doneButton;
    UIBarButtonItem *cancelButton;
    int currentStep;
}

@property (nonatomic, retain) IBOutlet TrackPadControl *tiltView;
@property (nonatomic, retain) IBOutlet UIBarButtonItem *doneButton;
@property (nonatomic, retain) IBOutlet UIBarButtonItem *cancelButton;

- (IBAction)done;
- (IBAction)cancel;

@end
