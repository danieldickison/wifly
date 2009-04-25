//
//  MainViewController.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "FlipsideViewController.h"

@interface MainViewController : UIViewController <FlipsideViewControllerDelegate>
{
    UIProgressView *rollIndicator;
    UIProgressView *pitchIndicator;
    UILabel *rollLabel;
    UILabel *pitchLabel;
}

@property (nonatomic, retain) IBOutlet UIProgressView *rollIndicator;
@property (nonatomic, retain) IBOutlet UIProgressView *pitchIndicator;
@property (nonatomic, retain) IBOutlet UILabel *rollLabel;
@property (nonatomic, retain) IBOutlet UILabel *pitchLabel;

- (IBAction)showInfo;

- (void)updatePitch:(float)pitch roll:(float)roll;

@end
