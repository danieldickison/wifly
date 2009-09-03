//
//  iX_YokeAppDelegate.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//


#define SharedAppDelegate ((iX_YokeAppDelegate *)[UIApplication sharedApplication].delegate)

extern NSString * const iXTiltUpdatedNotification;

@class MainViewController;
@class AsyncUdpSocket;

@interface iX_YokeAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate>
{
    UIWindow *window;
    MainViewController *mainViewController;
    
    float acceleration[3];
    float centerTiltRotationMatrix[9];
    
    float touch_x;
    float touch_y;
    float tilt_x;
    float tilt_y;
    
    NSString *hostAddress;
    unsigned hostPort;
    AsyncUdpSocket *socket;
    unsigned long packetTag;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;
@property (nonatomic, copy) NSString *hostAddress;
@property (nonatomic, assign) unsigned hostPort;

// These should always each be in the range [0, 1]
@property (nonatomic, assign) float touch_x;
@property (nonatomic, assign) float touch_y;
@property (nonatomic, assign) float tilt_x;
@property (nonatomic, assign) float tilt_y;

// Sets the current tilt as the "center".
- (void)resetTiltCenter;

@end

