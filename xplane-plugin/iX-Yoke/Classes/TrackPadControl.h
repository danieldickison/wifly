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
    TrackPadInteractionMode interactionMode;
    CGFloat pointRadius;
}

@property (nonatomic, assign) float xValue;
@property (nonatomic, assign) float yValue;
@property (nonatomic, assign) TrackPadInteractionMode interactionMode;
@property (nonatomic, assign) CGFloat pointRadius;

@end
