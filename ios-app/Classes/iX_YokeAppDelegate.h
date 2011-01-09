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
    BOOL auto_hold;
    BOOL auto_center_x;
    BOOL auto_center_y;
    
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
@property (nonatomic, assign) BOOL auto_hold;
@property (nonatomic, assign) BOOL auto_center_x;
@property (nonatomic, assign) BOOL auto_center_y;

- (void)getAccelerationVector:(float[3])outVector3;

- (void)setCalibrationWithCenterVector:(float[3])cv forwardVector:(float[3])fv;

@end

