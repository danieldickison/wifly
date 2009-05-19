//
//  TrackPadControl.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/29/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import <UIKit/UIKit.h>


typedef enum {
    TrackPadTouchesIgnored = 0,
    TrackPadTouchesValueRelative,
    TrackPadTouchesValueAbsolute,
    TrackPadTouchesBounds
} TrackPadInteractionMode;


@interface TrackPadControl : UIControl
{
    CGPoint minValue;
    CGPoint maxValue;
    CGPoint valuePoint;
    CGPoint holdPoint;
    BOOL holding;
    CGRect trackingBounds;
    TrackPadInteractionMode interactionMode;
    BOOL pinchTrackingX;
    BOOL pinchTrackingY;
}

@property (nonatomic, assign) CGFloat xValue;
@property (nonatomic, assign) CGFloat yValue;
@property (nonatomic, assign) CGRect trackingBounds;
@property (nonatomic, assign) TrackPadInteractionMode interactionMode;

- (void)setMinX:(CGFloat)minx maxX:(CGFloat)maxx minY:(CGFloat)miny maxY:(CGFloat)maxY;

// While "held", touches will adjust the tracking bounds instead of the value.  The raw-value crosshairs will keep updating, but the xValue and yValue properties will be pinned to the current values when it started holding.
@property (nonatomic, assign, getter=isHolding) BOOL holding;

@end
