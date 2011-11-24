//
//  BonjourViewController.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 11/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "BonjourViewController.h"
#import "iX_YokeAppDelegate.h"
#import "RemoteController.h"

@implementation BonjourViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    services = [[NSMutableArray alloc] init];
    browser = [[NSNetServiceBrowser alloc] init];
    browser.delegate = self;
    [browser searchForServicesOfType:@"_osc._udp." inDomain:@""];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    
    [browser stop];
    [browser release];
    browser = nil;
    [services release];
    services = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [services count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
    }
    
    NSNetService *service = [services objectAtIndex:indexPath.row];
    cell.textLabel.text = service.name;
    
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSNetService *service = [services objectAtIndex:indexPath.row];
    service.delegate = self;
    [service resolveWithTimeout:10];
}

- (void)netServiceDidResolveAddress:(NSNetService *)service
{
    SharedAppDelegate.remoteController.hostAddress = service.hostName;
    SharedAppDelegate.remoteController.hostPort = service.port;
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
    NSLog(@"Error with net service resolving: %@", errorDict);
}

#pragma mark - Net service browser delegate

- (void)netServiceBrowser:(NSNetServiceBrowser *)b didFindService:(NSNetService *)service moreComing:(BOOL)moreComing
{
    [self.tableView beginUpdates];
    [services addObject:service];
    [self.tableView insertRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:[services count]-1 inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
    [self.tableView endUpdates];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)b didRemoveService:(NSNetService *)service moreComing:(BOOL)moreComing
{
    [self.tableView beginUpdates];
    NSUInteger index = [services indexOfObject:service];
    [services removeObjectAtIndex:index];
    [self.tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:[NSIndexPath indexPathForRow:index inSection:0]] withRowAnimation:UITableViewRowAnimationTop];
    [self.tableView endUpdates];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didNotSearch:(NSDictionary *)errorDict
{
    NSLog(@"Error with net service browser: %@", errorDict);
}

@end
