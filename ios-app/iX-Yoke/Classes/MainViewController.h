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
}

@property (nonatomic, retain) IBOutlet TrackPadControl *trackpad;
@property (nonatomic, retain) IBOutlet TrackPadControl *tiltView;

- (IBAction)showInfo;
- (IBAction)trackpadUpdated;
- (IBAction)tiltHold:(UISwitch *)holdSwitch;

@end
