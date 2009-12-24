//
//  MainViewController.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "FlipsideViewController.h"

@class TrackPadControl;

@interface MainViewController : UIViewController <FlipsideViewControllerDelegate>
{
    TrackPadControl *trackpad;
    TrackPadControl *tiltView;
    UISwitch *holdSwitch;
    UISwitch *autoHoldSwitch;
    int trackpadTouchCount;
}

@property (nonatomic, retain) IBOutlet TrackPadControl *trackpad;
@property (nonatomic, retain) IBOutlet TrackPadControl *tiltView;
@property (nonatomic, retain) IBOutlet UISwitch *holdSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *autoHoldSwitch;

- (IBAction)showInfo;
- (IBAction)trackpadUpdated;
- (IBAction)trackpadTouchDown;
- (IBAction)trackpadTouchUp;
- (IBAction)tiltHold;
- (IBAction)autoHold;

@end
