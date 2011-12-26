//
//  LandscapeViewController.h
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FlipsideViewController.h"

@class TrackPadControl;

@interface LandscapeViewController : UIViewController <FlipsideViewControllerDelegate>
{
    BOOL autoHold;
    NSUInteger autoHoldTrigger;
    BOOL touchingLeft;
    BOOL touchingRight;
    
    id tiltObserver;
}

@property (nonatomic, assign) IBOutlet TrackPadControl *leftTrackPad;
@property (nonatomic, assign) IBOutlet TrackPadControl *rightTrackPad;
@property (nonatomic, assign) IBOutlet MultiStateButton *tiltButton;
@property (nonatomic, assign) IBOutlet TrackPadControl *tiltIndicator;

- (IBAction)infoButtonAction;
- (IBAction)leftTrackPadChanged;
- (IBAction)leftTrackPadTouchDown;
- (IBAction)leftTrackPadTouchUp;
- (IBAction)rightTrackPadChanged;
- (IBAction)rightTrackPadTouchDown;
- (IBAction)rightTrackPadTouchUp;
- (IBAction)tiltHoldAction;

- (IBAction)triggerButtonDown:(UIButton *)button;
- (IBAction)triggerButtonUp:(UIButton *)button;

- (UIControlState)tiltButtonState;
- (void)updateAutoHold;

@end
