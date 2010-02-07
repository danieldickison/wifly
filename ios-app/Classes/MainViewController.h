//
//  MainViewController.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "FlipsideViewController.h"

@class TrackPadControl;
@class MultiStateButton;

@interface MainViewController : UIViewController <FlipsideViewControllerDelegate>
{
    TrackPadControl *trackpad;
    TrackPadControl *tiltView;
    MultiStateButton *holdButton;
    MultiStateButton *autoCenterButton;
    int trackpadTouchCount;
}

@property (nonatomic, retain) IBOutlet TrackPadControl *trackpad;
@property (nonatomic, retain) IBOutlet TrackPadControl *tiltView;
@property (nonatomic, retain) IBOutlet MultiStateButton *holdButton;
@property (nonatomic, retain) IBOutlet MultiStateButton *autoCenterButton;

- (IBAction)showInfo;
- (IBAction)trackpadUpdated;
- (IBAction)trackpadTouchDown;
- (IBAction)trackpadTouchUp;
- (IBAction)holdAction;
- (IBAction)autoCenterAction;

@end
