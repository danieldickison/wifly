//
//  MultiStateButton.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 2/7/10.
//  Copyright 2010 Daniel_Dickison. All rights reserved.
//

#import "MultiStateButton.h"

@implementation MultiStateButton

- (UIControlState)customStates
{
    return customStates;
}

- (void)setCustomStates:(UIControlState)states
{
    customStates = states;
    
    // Hack to force UIButton to re-read the state property.
    self.enabled = NO;
    self.enabled = YES;
    
    [self setNeedsDisplay];
}

- (UIControlState)state
{
    return [super state] | customStates;
}

@end
