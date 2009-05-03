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
    TrackPadControl *calibrationTrackPad;
    TrackPadControl *controlTrackPad;
}

- (IBAction)showInfo;
- (IBAction)toggleSuspend:(UISwitch *)sender;
- (IBAction)switchTrackPad:(UISegmentedControl *)sender;

- (IBAction)resetCenter;

- (void)updatePitch:(float *)ioPitch roll:(float *)ioRoll;

@end
