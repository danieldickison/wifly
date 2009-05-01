//
//  TrackPadControl.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/29/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface TrackPadControl : UIControl
{
    CGPoint minValue;
    CGPoint maxValue;
    CGPoint valuePoint;
    CGRect trackingBounds;
    BOOL absoluteTracking;
    BOOL singleTouchCenters;
    BOOL pinchTrackingX;
    BOOL pinchTrackingY;
}

@property (nonatomic, assign) CGFloat xValue;
@property (nonatomic, assign) CGFloat yValue;;
@property (nonatomic, assign) CGRect trackingBounds;

// If absoluteTracking is true, the value point is placed exactly where single-finger touches occur rather than depending on the relative movement of drags.
@property (nonatomic, assign) BOOL absoluteTracking;

// If this is true, single touch and drag moves the center of the tracking bounds instead of setting the value point.  In this case, it's expected the value is set programatically by xValue and yValue (e.g. from accellerometer inputs).
@property (nonatomic, assign) BOOL singleTouchCenters;

- (void)setMinX:(CGFloat)minx maxX:(CGFloat)maxx minY:(CGFloat)miny maxY:(CGFloat)maxY;

@end
