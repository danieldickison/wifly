//
//  iX_YokeAppDelegate.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//

@class MainViewController;

@interface iX_YokeAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate>
{
    UIWindow *window;
    MainViewController *mainViewController;
    
    double xAvg;
    double yAvg;
    double zAvg;
    double pitch;
    double roll;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;

@end

