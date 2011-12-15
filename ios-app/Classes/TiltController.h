//
//  TiltController.h
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

extern NSString * const iXTiltUpdatedNotification;

@interface TiltController : NSObject <UIAccelerometerDelegate>
{
    float acceleration[3];
    float portraitTransformMatrix[9];
    
    float tilt_x;
    float tilt_y;
    float center_x;
    float center_y;
}

@property (nonatomic, assign, readonly) float x;
@property (nonatomic, assign, readonly) float y;
@property (nonatomic, assign, readonly) float hold_x;
@property (nonatomic, assign, readonly) float hold_y;
@property (nonatomic, assign, getter=isHolding) BOOL hold;
@property (nonatomic, assign, getter=isLandscape) BOOL landscape;
@property (nonatomic, assign) BOOL centerOnHold;

- (void)center;

- (void)getAccelerationVector:(float[3])outVector3;
- (void)setPortraitCalibrationWithCenterVector:(float[3])cv forwardVector:(float[3])fv;

@end
