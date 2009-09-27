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
    TrackPadTouchesValueAbsolute
} TrackPadInteractionMode;


@interface TrackPadControl : UIControl
{
    CGPoint valuePoint;
    CGPoint crosshairPoint;
    TrackPadInteractionMode interactionMode;
    CGFloat pointRadius;
}

@property (nonatomic, assign) float xValue;
@property (nonatomic, assign) float yValue;
@property (nonatomic, assign) TrackPadInteractionMode interactionMode;
@property (nonatomic, assign) CGFloat pointRadius;

// Use this if you want the crosshairs to be different from the value point.
- (void)setXValue:(float)x yValue:(float)y xCrosshair:(float)cx yCrosshair:(float)cy;

@end
