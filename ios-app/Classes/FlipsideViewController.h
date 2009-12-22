//
//  FlipsideViewController.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

@protocol FlipsideViewControllerDelegate;
@class TrackPadControl;


@interface FlipsideViewController : UIViewController <UITextFieldDelegate>
{
	id <FlipsideViewControllerDelegate> delegate;
    UITextField *ipField;
    UITextField *portField;
    TrackPadControl *tiltView;
    UIBarButtonItem *doneButton;
    UIBarButtonItem *helpButton;
}

@property (nonatomic, assign) id <FlipsideViewControllerDelegate> delegate;
@property (nonatomic, retain) IBOutlet UITextField *ipField;
@property (nonatomic, retain) IBOutlet UITextField *portField;
@property (nonatomic, retain) IBOutlet TrackPadControl *tiltView;
@property (nonatomic, retain) IBOutlet UIBarButtonItem *doneButton;
@property (nonatomic, retain) IBOutlet UIBarButtonItem *helpButton;
- (IBAction)done;
- (IBAction)calibrate;
- (IBAction)ipFieldChanged;
- (IBAction)portFieldChanged;
- (IBAction)showHelp;


@end


@protocol FlipsideViewControllerDelegate
- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
@end

