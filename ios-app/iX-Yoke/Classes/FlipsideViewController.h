//
//  FlipsideViewController.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

@protocol FlipsideViewControllerDelegate;
@class TrackPadControl;


@interface FlipsideViewController : UIViewController
{
	id <FlipsideViewControllerDelegate> delegate;
    UITextField *ipField;
    UITextField *portField;
    TrackPadControl *tiltView;
}

@property (nonatomic, assign) id <FlipsideViewControllerDelegate> delegate;
@property (nonatomic, retain) IBOutlet UITextField *ipField;
@property (nonatomic, retain) IBOutlet UITextField *portField;
@property (nonatomic, retain) IBOutlet TrackPadControl *tiltView;
- (IBAction)done;
- (IBAction)setTiltCenter;


@end


@protocol FlipsideViewControllerDelegate
- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
@end

