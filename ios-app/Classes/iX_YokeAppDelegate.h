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
    float tiltTransformMatrix[9];
    
    float touch_x;
    float touch_y;
    float tilt_x;
    float tilt_y;
    float tilt_hold_x;
    float tilt_hold_y;
    BOOL tilt_hold;
    
    NSString *hostAddress;
    unsigned hostPort;
    AsyncUdpSocket *socket;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) MainViewController *mainViewController;
@property (nonatomic, copy) NSString *hostAddress;
@property (nonatomic, assign) unsigned hostPort;

// These should always each be in the range [0, 1]
@property (nonatomic, assign) float touch_x;
@property (nonatomic, assign) float touch_y;
@property (nonatomic, assign, readonly) float tilt_x;
@property (nonatomic, assign, readonly) float tilt_y;
@property (nonatomic, assign, readonly) float tilt_hold_x;
@property (nonatomic, assign, readonly) float tilt_hold_y;
@property (nonatomic, assign) BOOL tilt_hold;

- (void)getAccelerationVector:(float[3])outVector3;

- (void)setCalibrationWithCenterVector:(float[3])cv forwardVector:(float[3])fv;

@end

