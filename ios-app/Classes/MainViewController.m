//Copyright (c) 2011 Daniel Dickison
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#import "MainViewController.h"
#import "iX_YokeAppDelegate.h"
#import "TrackPadControl.h"
#import "MultiStateButton.h"


enum {
    kAutoCenterStateX = 1 << 16, // First bit of UIControlStateApplication
    kAutoCenterStateY = 1 << 17
};


@interface MainViewController ()

@property (nonatomic, readonly) UIControlState holdButtonState;
@property (nonatomic, readonly) UIControlState autoCenterButtonState;

@end



@implementation MainViewController

@synthesize trackpad, tiltView, holdButton, autoCenterButton;

-(void)dealloc
{
    [super dealloc];
}


 - (void)viewDidLoad
{
    [super viewDidLoad];
    
    trackpad.interactionMode = TrackPadTouchesValueRelative;
    trackpad.xValue = SharedAppDelegate.remoteController.trackpad1_x;
    trackpad.yValue = SharedAppDelegate.remoteController.trackpad1_y;
    
    tiltView.interactionMode = TrackPadTouchesIgnored;
    tiltView.pointRadius = 5.0f;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tiltUpdated:) name:iXTiltUpdatedNotification object:nil];
    
    [holdButton setImage:[UIImage imageNamed:@"Toggle Button Tilt On.png"] forState:kHoldStateOn];
    [holdButton setImage:[UIImage imageNamed:@"Toggle Button Tilt Auto.png"] forState:kHoldStateAuto];
    [holdButton setImage:[UIImage imageNamed:@"Toggle Button Tilt Auto On.png"] forState:kHoldStateOn | kHoldStateAuto];
    holdButton.customStates = self.holdButtonState;
    
    [autoCenterButton setImage:[UIImage imageNamed:@"Toggle Button Center X.png"] forState:kAutoCenterStateX];
    [autoCenterButton setImage:[UIImage imageNamed:@"Toggle Button Center Y.png"] forState:kAutoCenterStateY];
    [autoCenterButton setImage:[UIImage imageNamed:@"Toggle Button Center X Y.png"] forState:kAutoCenterStateX | kAutoCenterStateY];
    autoCenterButton.customStates = self.autoCenterButtonState;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.trackpad = nil;
    self.tiltView = nil;
    self.holdButton = nil;
    self.autoCenterButton = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


- (void)tiltUpdated:(NSNotification *)notification
{
    TiltController *tilt = SharedAppDelegate.tiltController;
    [tiltView setXValue:tilt.hold_x yValue:(1.0f - tilt.hold_y) xCrosshair:tilt.x yCrosshair:(1.0f - tilt.y)];
}


- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller {
    
	[self dismissModalViewControllerAnimated:YES];
}


- (IBAction)showInfo {    
	
	FlipsideViewController *controller = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	controller.delegate = self;
    controller.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
    
    UINavigationController *navController = [[UINavigationController alloc] initWithRootViewController:controller];
    navController.navigationBar.barStyle = UIBarStyleBlack;
	[self presentModalViewController:navController animated:YES];
    
    [controller release];
	[navController release];
}



- (IBAction)trackpadUpdated
{
    SharedAppDelegate.remoteController.trackpad1_x = trackpad.xValue;
    SharedAppDelegate.remoteController.trackpad1_y = trackpad.yValue;
}


- (IBAction)trackpadTouchDown
{
    if (autoHold)
    {
        SharedAppDelegate.tiltController.hold = NO;
        holdButton.customStates = self.holdButtonState;
    }
}

- (IBAction)trackpadTouchUp
{
    if (autoHold)
    {
        SharedAppDelegate.tiltController.hold = YES;
        holdButton.customStates = self.holdButtonState;
    }
}



- (IBAction)holdAction
{
    TiltController *tilt = SharedAppDelegate.tiltController;
    if (autoHold)
    {
        autoHold = NO;
        tilt.hold = YES;
    }
    else if (tilt.hold)
    {
        autoHold = NO;
        tilt.hold = NO;
    }
    else
    {
        autoHold = YES;
        tilt.hold = YES;
    }
    holdButton.customStates = self.holdButtonState;
}

- (IBAction)autoCenterAction
{
    trackpad.autoCenterMode = (trackpad.autoCenterMode == TrackPadAutoCenterOff ?
                               TrackPadAutoCenterX :
                               trackpad.autoCenterMode == TrackPadAutoCenterX ?
                               TrackPadAutoCenterY :
                               trackpad.autoCenterMode == TrackPadAutoCenterY ?
                               TrackPadAutoCenterBoth :
                               TrackPadAutoCenterOff);
    autoCenterButton.customStates = self.autoCenterButtonState;
}


- (UIControlState)holdButtonState
{
    return ((SharedAppDelegate.tiltController.hold ? kHoldStateOn : 0) |
            (autoHold ? kHoldStateAuto : 0));
}

- (UIControlState)autoCenterButtonState
{
    return ((trackpad.autoCenterMode & TrackPadAutoCenterX ? kAutoCenterStateX : 0) |
            (trackpad.autoCenterMode & TrackPadAutoCenterY ? kAutoCenterStateY : 0));
}


@end
