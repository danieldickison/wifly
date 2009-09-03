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
}

@property (nonatomic, retain) IBOutlet TrackPadControl *trackpad;

- (IBAction)showInfo;
- (IBAction)toggleSuspend:(UISwitch *)sender;
- (IBAction)trackpadUpdated;

@end
