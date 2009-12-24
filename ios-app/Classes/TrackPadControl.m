//
//  TrackPadControl.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/29/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import "TrackPadControl.h"


static CGColorRef colorForActive(BOOL active, CGFloat alpha)
{
    CGFloat hue = 0.333;
    CGFloat saturation = (active ? 1.0 : 0.0);
    return [[UIColor colorWithHue:hue saturation:saturation brightness:1.0 alpha:alpha] CGColor];
}



@implementation TrackPadControl

@synthesize interactionMode, pointRadius;


- (void)dealloc
{
    [super dealloc];
}

- (id)initWithCoder:(NSCoder *)coder
{
    if (self = [super initWithCoder:coder])
    {
        pointRadius = 8.0f;
    }
    return self;
}


- (float)xValue
{
    return (valuePoint.x - pointRadius - CGRectGetMinX(self.bounds)) / (self.bounds.size.width - 2.0f*pointRadius);
}

- (float)yValue
{
    return (CGRectGetMaxY(self.bounds) - valuePoint.y - pointRadius) / (self.bounds.size.height - 2.0f*pointRadius);
}


- (void)setXValue:(float)x
{
    valuePoint.x = roundf(CGRectGetMinX(self.bounds) + pointRadius + (self.bounds.size.width - 2.0f*pointRadius) * x);
    crosshairPoint.x = valuePoint.x;
    [self setNeedsDisplay];
}

- (void)setYValue:(float)y
{
    valuePoint.y = roundf(CGRectGetMaxY(self.bounds) - pointRadius - (self.bounds.size.height - 2.0f*pointRadius) * y);
    crosshairPoint.y = valuePoint.y;
    [self setNeedsDisplay];
}


- (void)setXValue:(float)x yValue:(float)y xCrosshair:(float)cx yCrosshair:(float)cy
{
    self.xValue = x;
    self.yValue = y;
    crosshairPoint.x = roundf(CGRectGetMinX(self.bounds) + pointRadius + (self.bounds.size.width - 2.0f*pointRadius) * cx);
    crosshairPoint.y = roundf(CGRectGetMaxY(self.bounds) - pointRadius - (self.bounds.size.height - 2.0f*pointRadius) * cy);
}


- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGRect bounds = self.bounds;
    
    // Draw background...
    //CGContextSetFillColorWithColor(context, [[UIColor blackColor] CGColor]);   
    //CGContextFillRect(context, bounds);
    
    
    // Draw the unconstrained value as crosshairs...
    float valx = crosshairPoint.x;
    float valy = crosshairPoint.y;
    CGContextMoveToPoint(context, CGRectGetMinX(bounds), valy);
    CGContextAddLineToPoint(context, CGRectGetMaxX(bounds), valy);
    CGContextMoveToPoint(context, valx, CGRectGetMinY(bounds));
    CGContextAddLineToPoint(context, valx, CGRectGetMaxY(bounds));
    CGContextSetStrokeColorWithColor(context, [[UIColor colorWithWhite:1.0 alpha:0.5] CGColor]);
    CGContextStrokePath(context);
    
    
    // Draw the tracking bounds and center...
#define kCornerSize 10.0f
#define kCornerRadius 5.0f
#define kMidLength 5.0f
    CGRect effectiveBounds = CGRectInset(bounds, pointRadius, pointRadius);
    CGFloat xmin = CGRectGetMinX(effectiveBounds);
    CGFloat xmid = CGRectGetMidX(effectiveBounds);
    CGFloat xmax = CGRectGetMaxX(effectiveBounds);
    CGFloat ymin = CGRectGetMinY(effectiveBounds);
    CGFloat ymid = CGRectGetMidY(effectiveBounds);
    CGFloat ymax = CGRectGetMaxY(effectiveBounds);
    
    // Top
    CGContextMoveToPoint(context, xmin, ymin + kCornerSize);
    CGContextAddArcToPoint(context, xmin, ymin, xmin + kCornerRadius, ymin, kCornerRadius);
    CGContextAddLineToPoint(context, xmin + kCornerSize, ymin);
    CGContextMoveToPoint(context, xmid - kMidLength, ymin);
    CGContextAddLineToPoint(context, xmid + kMidLength, ymin);
    
    // Right
    CGContextMoveToPoint(context, xmax - kCornerSize, ymin);
    CGContextAddArcToPoint(context, xmax, ymin, xmax, ymin + kCornerRadius, kCornerRadius);
    CGContextAddLineToPoint(context, xmax, ymin + kCornerSize);
    CGContextMoveToPoint(context, xmax, ymid - kMidLength);
    CGContextAddLineToPoint(context, xmax, ymid + kMidLength);
    
    // Bottom
    CGContextMoveToPoint(context, xmax, ymax - kCornerSize);
    CGContextAddArcToPoint(context, xmax, ymax, xmax - kCornerRadius, ymax, kCornerRadius);
    CGContextAddLineToPoint(context, xmax - kCornerSize, ymax);
    CGContextMoveToPoint(context, xmid + kMidLength, ymax);
    CGContextAddLineToPoint(context, xmid - kMidLength, ymax);
    
    // Left
    CGContextMoveToPoint(context, xmin + kCornerSize, ymax);
    CGContextAddArcToPoint(context, xmin, ymax, xmin, ymax - kCornerRadius, kCornerRadius);
    CGContextAddLineToPoint(context, xmin, ymax - kCornerSize);
    CGContextMoveToPoint(context, xmin, ymid + kMidLength);
    CGContextAddLineToPoint(context, xmin, ymid - kMidLength);
    
    // Center
    CGContextMoveToPoint(context, xmid, ymid - kMidLength);
    CGContextAddLineToPoint(context, xmid, ymid + kMidLength);
    CGContextMoveToPoint(context, xmid - kMidLength, ymid);
    CGContextAddLineToPoint(context, xmid + kMidLength, ymid);
    
    // Draw it
    CGContextSetStrokeColorWithColor(context, colorForActive(NO, 0.9));
    CGContextSetLineWidth(context, 1.0);
    CGContextStrokePath(context);
    
    
    
    // Draw the constrained value as a bright dot...
    CGRect valueRect = CGRectInset(CGRectMake(valuePoint.x, valuePoint.y, 0, 0), -pointRadius, -pointRadius);
    CGContextAddEllipseInRect(context, valueRect);
    BOOL valueActive = interactionMode != TrackPadTouchesIgnored;
    CGContextSetStrokeColorWithColor(context, colorForActive(valueActive, 0.85));
    CGContextSetFillColorWithColor(context, colorForActive(valueActive, 0.5));
    CGContextSetLineWidth(context, 1.0);
    
    CGContextDrawPath(context, kCGPathFillStroke);
}



#pragma mark -
#pragma mark Event Handling

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (interactionMode == TrackPadTouchesIgnored) return;
    
    for (int i = 0; i < [touches count]; i++)
    {
        [self sendActionsForControlEvents:UIControlEventTouchDown];
    }
    
    NSSet *myTouches = [event touchesForView:self];
    // Single-touch for moving; two-fingers used to be for zooming...not any more.
    if ([myTouches count] == 1)
    {
        UITouch *touch = [myTouches anyObject];
        if (touch.tapCount == 1)
        {
            if (interactionMode == TrackPadTouchesValueAbsolute)
            {
                valuePoint = [touch locationInView:self];
                valuePoint.x = MIN(CGRectGetMaxX(self.bounds) - pointRadius, MAX(CGRectGetMinX(self.bounds) + pointRadius, valuePoint.x));
                valuePoint.y = MIN(CGRectGetMaxY(self.bounds) - pointRadius, MAX(CGRectGetMinY(self.bounds) + pointRadius, valuePoint.y));
                crosshairPoint.x = valuePoint.x;
                crosshairPoint.y = valuePoint.y;
                [self setNeedsDisplay];
                [self sendActionsForControlEvents:UIControlEventValueChanged];
            }
        }
    }
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
    if (interactionMode == TrackPadTouchesIgnored) return;
    
    NSSet *myTouches = [event touchesForView:self];
    if ([myTouches count] == 1)
    {
        UITouch *touch = [myTouches anyObject];
        if (interactionMode == TrackPadTouchesValueAbsolute)
        {
            valuePoint = [touch locationInView:self];
        }
        else
        {
            CGPoint point = [touch locationInView:self];
            CGPoint prevPoint = [touch previousLocationInView:self];
            CGFloat dx = point.x - prevPoint.x;
            CGFloat dy = point.y - prevPoint.y;
            valuePoint.x += dx;
            valuePoint.y += dy;
        }
        valuePoint.x = MIN(CGRectGetMaxX(self.bounds) - pointRadius, MAX(CGRectGetMinX(self.bounds) + pointRadius, valuePoint.x));
        valuePoint.y = MIN(CGRectGetMaxY(self.bounds) - pointRadius, MAX(CGRectGetMinY(self.bounds) + pointRadius, valuePoint.y));
        crosshairPoint.x = valuePoint.x;
        crosshairPoint.y = valuePoint.y;
        [self setNeedsDisplay];
        [self sendActionsForControlEvents:UIControlEventValueChanged];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (int i = 0; i < [touches count]; i++)
    {
        [self sendActionsForControlEvents:UIControlEventTouchUpInside];
    }
}

@end
