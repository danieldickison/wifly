//
//  BonjourViewController.h
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 11/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface BonjourViewController : UITableViewController <NSNetServiceBrowserDelegate, NSNetServiceDelegate> {
    NSNetServiceBrowser *browser;
    NSMutableArray *services;
}
@end
