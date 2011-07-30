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

#import <UIKit/UIKit.h>


typedef enum {
    TrackPadTouchesIgnored = 0,
    TrackPadTouchesValueRelative,
    TrackPadTouchesValueAbsolute
} TrackPadInteractionMode;

typedef enum {
    TrackPadAutoCenterOff = 0,
    TrackPadAutoCenterX = 1 << 0,
    TrackPadAutoCenterY = 1 << 1,
    TrackPadAutoCenterBoth = TrackPadAutoCenterX | TrackPadAutoCenterY
} TrackPadAutoCenterMode;


@interface TrackPadControl : UIControl
{
    CGPoint valuePoint;
    CGPoint crosshairPoint;
    TrackPadInteractionMode interactionMode;
    TrackPadAutoCenterMode autoCenterMode;
    CGFloat pointRadius;
    int touchCount;
}

@property (nonatomic, assign) float xValue;
@property (nonatomic, assign) float yValue;
@property (nonatomic, assign) TrackPadInteractionMode interactionMode;
@property (nonatomic, assign) TrackPadAutoCenterMode autoCenterMode;
@property (nonatomic, assign) CGFloat pointRadius;

// Use this if you want the crosshairs to be different from the value point.
- (void)setXValue:(float)x yValue:(float)y xCrosshair:(float)cx yCrosshair:(float)cy;

@end
