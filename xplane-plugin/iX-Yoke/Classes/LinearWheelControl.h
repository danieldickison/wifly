//
//  LinearWheelControl.h
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/29/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef enum {
    LinearWheelStyleHorizontal,
    LinearWheelStyleVertical,
    LinearWheelStyleClockwise,
    LinearWheelStyleCounterClockwise
} LinearWheelStyle;


@interface LinearWheelControl : UIControl
{
    CGFloat value;
    CGFloat minValue;
    CGFloat maxValue;
    CGFloat trackingRange;
    
    BOOL trackHorizontalDrag;
    BOOL trackVerticalDrag;
    UIImage *backgroundImage;
    UIImage *thumbImage;
    CGRect indicatorBounds;
    LinearWheelStyle style;
}

@property (nonatomic, assign) CGFloat value;
@property (nonatomic, assign) CGFloat minValue;
@property (nonatomic, assign) CGFloat maxValue;

// Tracking range is the number of pixel the user must drag in order to cover the full range of values (from minValue to maxValue).  Setting this to (indicatorBounds.size.width - thumbImage.size.width) will resemble standard slider behavior, where the thumb moves the same distance as the drag.
@property (nonatomic, assign) CGFloat trackingRange;

@property (nonatomic, assign) BOOL trackHorizontalDrag;
@property (nonatomic, assign) BOOL trackVerticalDrag;
@property (nonatomic, retain) UIImage *backgroundImage;
@property (nonatomic, retain) UIImage *thumbImage;
@property (nonatomic, assign) CGRect indicatorBounds;
@property (nonatomic, assign) LinearWheelStyle style;

@end
