//
//  TrackPadControl.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 4/29/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import "TrackPadControl.h"

@interface TrackPadControl ()

@property (readonly) CGPoint constrainedPoint;

@end



@implementation TrackPadControl

@synthesize trackingBounds, absoluteTracking;
@dynamic xValue, yValue, constrainedPoint;


// Tracking bounds mirrors view bounds unless subsequently overridden.
- (void)setFrame:(CGRect)rect
{
    [super setFrame:rect];
    self.trackingBounds = CGRectInset(self.bounds, 10, 10);
}

- (void)setBounds:(CGRect)rect
{
    [super setBounds:rect];
    self.trackingBounds = CGRectInset(self.bounds, 10, 10);
}


- (void)setMinX:(CGFloat)minx maxX:(CGFloat)maxx minY:(CGFloat)miny maxY:(CGFloat)maxy
{
    minValue = CGPointMake(minx, miny);
    maxValue = CGPointMake(maxx, maxy);
}


- (CGFloat)xValue
{
    return minValue.x + ((maxValue.x - minValue.x) * (self.constrainedPoint.x - trackingBounds.origin.x) / trackingBounds.size.width);
}

- (void)setXValue:(CGFloat)x
{
    valuePoint.x = self.bounds.origin.x + (self.bounds.size.width * (x - minValue.x) / (maxValue.x - minValue.x));
    [self setNeedsDisplay];
}

- (CGFloat)yValue
{
    // Remember that the y coordinates are flipped...
    return minValue.y + ((maxValue.y - minValue.y) * (CGRectGetMaxY(trackingBounds) - self.constrainedPoint.y) / trackingBounds.size.height);
}

- (void)setYValue:(CGFloat)y
{
    valuePoint.y = CGRectGetMaxY(self.bounds) - (self.bounds.size.height * (y - minValue.y) / (maxValue.y - minValue.y));
    [self setNeedsDisplay];
}


// If the point is outside the tracking bounds, we'll compute where the line from the center of the tracking bounds to the unconstrained point intersects the bounding box.
- (CGPoint)constrainedPoint
{
    if (CGRectContainsPoint(trackingBounds, valuePoint))
        return valuePoint;
    
    return CGPointMake(MIN(MAX(valuePoint.x, CGRectGetMinX(trackingBounds)),
                           CGRectGetMaxX(trackingBounds)),
                       MIN(MAX(valuePoint.y, CGRectGetMinY(trackingBounds)),
                           CGRectGetMaxY(trackingBounds)));
}



- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGRect bounds = self.bounds;
    
    // Draw background...
    CGContextSetFillColorWithColor(context, [[UIColor blackColor] CGColor]);   
    CGContextFillRect(context, bounds);
    
    
    // Draw the unconstrained value as crosshairs...
    CGContextMoveToPoint(context, CGRectGetMinX(bounds), valuePoint.y);
    CGContextAddLineToPoint(context, CGRectGetMaxX(bounds), valuePoint.y);
    CGContextMoveToPoint(context, valuePoint.x, CGRectGetMinY(bounds));
    CGContextAddLineToPoint(context, valuePoint.x, CGRectGetMaxY(bounds));
    CGContextSetStrokeColorWithColor(context, [[UIColor colorWithWhite:1.0 alpha:0.5] CGColor]);
    CGContextStrokePath(context);
    
    
    // Draw the tracking bounds and center...
#define kCornerSize 10.0f
#define kCornerRadius 5.0f
#define kMidLength 5.0f
    CGFloat xmin = CGRectGetMinX(trackingBounds);
    CGFloat xmid = CGRectGetMidX(trackingBounds);
    CGFloat xmax = CGRectGetMaxX(trackingBounds);
    CGFloat ymin = CGRectGetMinY(trackingBounds);
    CGFloat ymid = CGRectGetMidY(trackingBounds);
    CGFloat ymax = CGRectGetMaxY(trackingBounds);
    
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
    CGContextSetStrokeColorWithColor(context, [[UIColor greenColor] CGColor]);
    CGContextStrokePath(context);
    
    
    
    // Draw the constrained value as a bright dot...
#define kConstrainedValueRadius 8.0f
    CGPoint constrained = self.constrainedPoint;
    CGRect valueRect = CGRectInset(CGRectMake(constrained.x, constrained.y, 0, 0), -kConstrainedValueRadius, -kConstrainedValueRadius);
    CGContextAddEllipseInRect(context, valueRect);
    CGContextSetStrokeColorWithColor(context, [[UIColor colorWithWhite:1.0 alpha:0.85] CGColor]);
    CGContextSetFillColorWithColor(context, [[UIColor colorWithWhite:1.0 alpha:0.5] CGColor]);
    CGContextDrawPath(context, kCGPathFillStroke);
}



#pragma mark -
#pragma mark Event Handling

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    NSSet *myTouches = [event touchesForView:self];
    if ([myTouches count] == 1)
    {
        [super touchesBegan:touches withEvent:event];
    }
    else if ([myTouches count] == 2) // zoom time!
    {
        NSArray *twoTouches = [myTouches allObjects];
        CGPoint loc1 = [[twoTouches objectAtIndex:0] locationInView:self];
        CGPoint loc2 = [[twoTouches objectAtIndex:1] locationInView:self];
        pinchTrackingX = (fabsf(loc1.x - loc2.x) > 75);
        pinchTrackingY = (fabsf(loc1.y - loc2.y) > 75);
    }
    // I don't think it's worth trying to make 3+ touches work.
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
    NSSet *myTouches = [event touchesForView:self];
    if ([myTouches count] == 1)
    {
        [super touchesMoved:touches withEvent:event];
    }
    else if ([myTouches count] == 2) // zoom time!
    {
        NSArray *twoTouches = [myTouches allObjects];
        CGPoint loc1 = [[twoTouches objectAtIndex:0] locationInView:self];
        CGPoint loc2 = [[twoTouches objectAtIndex:1] locationInView:self];
        if (pinchTrackingX)
        {
            trackingBounds.origin.x = MIN(loc1.x, loc2.x);
            trackingBounds.size.width = fabs(loc1.x - loc2.x);
        }
        if (pinchTrackingY)
        {
            trackingBounds.origin.y = MIN(loc1.y, loc2.y);
            trackingBounds.size.height = fabs(loc1.y - loc2.y);
        }
        self.trackingBounds = CGRectIntersection(trackingBounds, self.bounds);
        [self setNeedsDisplay];
        [self sendActionsForControlEvents:UIControlEventValueChanged];
    }
}


- (BOOL)beginTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    if (absoluteTracking)
    {
        valuePoint = [touch locationInView:self];
        [self setNeedsDisplay];
        [self sendActionsForControlEvents:UIControlEventValueChanged];
    }
    return YES;
}

- (BOOL)continueTrackingWithTouch:(UITouch *)touch withEvent:(UIEvent *)event
{
    if (absoluteTracking)
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
        
        // For relative tracking, don't let the crosshairs ever leave the tracking bounds.
        valuePoint = self.constrainedPoint;
    }
    [self setNeedsDisplay];
    [self sendActionsForControlEvents:UIControlEventValueChanged];
    return YES;
}


@end
