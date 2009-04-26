//
//  FlipsideViewController.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

#import "FlipsideViewController.h"
#import "iX_YokeAppDelegate.h"


@implementation FlipsideViewController

@synthesize delegate, ipField, portField;


- (void)dealloc
{
    self.ipField = nil;
    self.portField = nil;
    [super dealloc];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    ipField.text = SharedAppDelegate.hostAddress;
    portField.text =
        (SharedAppDelegate.hostPort == 0 ? nil
         : [NSString stringWithFormat:@"%d", SharedAppDelegate.hostPort]);
}


- (IBAction)done
{
    SharedAppDelegate.hostAddress = ipField.text;
    SharedAppDelegate.hostPort = [portField.text intValue];
	[self.delegate flipsideViewControllerDidFinish:self];	
}



@end
