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
