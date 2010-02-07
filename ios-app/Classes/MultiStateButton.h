//
//  MultiStateButton.h
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 2/7/10.
//  Copyright 2010 Daniel_Dickison. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface MultiStateButton : UIButton
{
    UIControlState customStates;
}

@property (nonatomic, assign) UIControlState customStates;
- (UIControlState)state;

@end
