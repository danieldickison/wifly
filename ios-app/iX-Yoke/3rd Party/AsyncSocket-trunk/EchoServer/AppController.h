#import <Cocoa/Cocoa.h>

@class AsyncSocket;
@class AsyncUdpSocket;

@interface AppController : NSObject
{
	AsyncSocket *listenSocket;
	NSMutableArray *connectedSockets;
    AsyncUdpSocket *udpSocket;
    long udpPacketTag;
	
	BOOL isRunning;
	
    IBOutlet id logView;
    IBOutlet id portField;
    IBOutlet id startStopButton;
    IBOutlet id protocolRadioMatrix;
}
- (IBAction)startStop:(id)sender;
@end
