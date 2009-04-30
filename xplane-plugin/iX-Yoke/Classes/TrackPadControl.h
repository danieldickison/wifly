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
    BOOL pinchTrackingX;
    BOOL pinchTrackingY;
}

@property (nonatomic, assign) CGFloat xValue;
@property (nonatomic, assign) CGFloat yValue;;
@property (nonatomic, assign) CGRect trackingBounds;
@property (nonatomic, assign) BOOL absoluteTracking;

- (void)setMinX:(CGFloat)minx maxX:(CGFloat)maxx minY:(CGFloat)miny maxY:(CGFloat)maxY;

@end
