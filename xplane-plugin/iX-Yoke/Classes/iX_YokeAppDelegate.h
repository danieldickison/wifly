//
//  iX_YokeAppDelegate.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/25/09.
//  Copyright Daniel_Dickison 2009. All rights reserved.
//


#define SharedAppDelegate ((iX_YokeAppDelegate *)[UIApplication sharedApplication].delegate)


@class MainViewController;
@class AsyncUdpSocket;

@interface iX_YokeAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate>
{
    UIWindow *window;
    MainViewController *mainViewController;
    
    BOOL suspended;
    
    float xAvg;
    float yAvg;
    float zAvg;
    
    float pitchOffset;
    float rollOffset;
    
    float touch_x;
    float touch_y;
    
    NSString *hostAddress;
    unsigned hostPort;
    AsyncUdpSocket *socket;
    unsigned long packetTag;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;
@property (nonatomic, assign, getter=isSuspended) BOOL suspended;
@property (nonatomic, copy) NSString *hostAddress;
@property (nonatomic, assign) unsigned hostPort;
@property (nonatomic, assign) float touch_x;
@property (nonatomic, assign) float touch_y;

// Sets the current tilt as the "center".
- (void)resetCalibration;

@end

