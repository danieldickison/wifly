//
//  LinearWheelControl.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/29/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import "LinearWheelControl.h"
#import <math.h>


@interface LinearWheelControl ()

- (void)initIvars;
- (void)drawThumb;
- (void)drawCircularIndicator;

@end




@implementation LinearWheelControl

@synthesize value, minValue, maxValue, trackingRange, trackHorizontalDrag, trackVerticalDrag, backgroundImage, thumbImage, indicatorBounds, style;


#pragma mark -
#pragma mark Initialization

- (id)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        [self initIvars];
    }
    return self;
}


- (id)initWithCoder:(NSCoder *)coder
{
    if (self = [super initWithCoder:coder])
    {
        [self initIvars];
    }
    return self;
}


- (void)initIvars
{
    value = 0.5;
    minValue = 0.0;
    maxValue = 1.0;
    trackHorizontalDrag = YES;
    trackVerticalDrag = NO;
    indicatorBounds = self.bounds;
    style = LinearWheelStyleHorizontal;
    self.thumbImage = [UIImage imageNamed:@"Default Linear Wheel Thumb.png"];
    self.backgroundImage = [[UIImage imageNamed:@"Default Linear Wheel BG.png"] stretchableImageWithLeftCapWidth:14 topCapHeight:14];
    trackingRange = indicatorBounds.size.width - thumbImage.size.width;
}

- (void)dealloc
{
    self.backgroundImage = nil;
    self.thumbImage = nil;
    [super dealloc];
}




#pragma mark -
#pragma mark Drawing


- (void)drawRect:(CGRect)rect
{
    // Possible optimization is to put the background image in a separate layer so it doesn't have to be redrawn every time.
    
    CGRect bounds = self.bounds;
    [backgroundImage drawInRect:bounds];
    switch (style)
    {
        case LinearWheelStyleHorizontal:
        case LinearWheelStyleVertical:
            [self drawThumb];
            break;
            
        case LinearWheelStyleClockwise:
        case LinearWheelStyleCounterClockwise:
            [self drawCircularIndicator];
            break;
    }
}



- (void)drawThumb
{
    CGFloat ratio = (value - minValue) / (maxValue - minValue);
    CGFloat boundsRange, imgRange, offset;
    CGRect imgFrame;
    
    if (style == LinearWheelStyleHorizontal)
    {
        boundsRange = indicatorBounds.size.width;
        imgRange = thumbImage.size.width;
        offset = indicatorBounds.origin.x + ratio * (boundsRange - imgRange);
        imgFrame = CGRectMake(offset,
                              indicatorBounds.origin.y,
                              thumbImage.size.width,
                              indicatorBounds.size.height);
    }
    else
    {
        boundsRange = indicatorBounds.size.height;
        imgRange = thumbImage.size.height;
        offset = indicatorBounds.size.height + indicatorBounds.origin.y - ratio * (boundsRange - imgRange) - imgRange;
        imgFrame = CGRectMake(indicatorBounds.origin.x,
                              offset,
                              indicatorBounds.size.width,
                              thumbImage.size.height);
    }
    [thumbImage drawInRect:imgFrame];
}

- (void)drawCircularIndicator
{
    //TODO
}



#pragma mark -
#pragma mark Event Handling

- (BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    return trackVerticalDrag || trackHorizontalDrag;
}

- (BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    CGPoint point = [touch locationInView:self];
    CGPoint prevPoint = [touch previousLocationInView:self];
    CGFloat dx = point.x - prevPoint.x;
    CGFloat dy = point.y - prevPoint.y;
    CGFloat delta;
    if (trackVerticalDrag && trackHorizontalDrag)
    {
        if (fabsf(dx) > fabsf(dy))
            delta = dx;
        else
            delta = -dy;
    }
    else if (trackVerticalDrag)
        delta = -dy;
    else if (trackHorizontalDrag)
        delta = dx;
    else
        return NO;
    
    value += (delta / trackingRange) * (maxValue - minValue);
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;
    [self setNeedsDisplay];
    [self sendActionsForControlEvents:UIControlEventValueChanged];
    return YES;
}



@end
