//
//  HelpViewController.m
//  iX-Yoke
//
//  Created by Daniel Dickison on 9/4/09.
//  Copyright 2009 Daniel_Dickison. All rights reserved.
//

#import "HelpViewController.h"


@implementation HelpViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
    {
        self.title = NSLocalizedString(@"Help", @"");
    }
    return self;
}


- (void)loadView
{
    UIWebView *webview = [[UIWebView alloc] init];
    webview.delegate = self;
    webview.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [webview loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://danieldickison.com/wifly/iphone-help"] cachePolicy:NSURLCacheStorageAllowedInMemoryOnly timeoutInterval:5.0f]];
    self.view = webview;
    [webview release];
}


- (void)webViewDidStartLoad:(UIWebView *)webView
{
    [UIApplication sharedApplication].networkActivityIndicatorVisible = YES;
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    [webView loadRequest:[NSURLRequest requestWithURL:[NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"index" ofType:@"html" inDirectory:@"local-help"]]]];
}

@end
