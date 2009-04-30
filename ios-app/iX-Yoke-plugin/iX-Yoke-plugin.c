/*
 *  iX-Yoke-plugin.cpp
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 4/26/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */

/* Uncomment the following line ONLY if you NEED v2 ONLY APIs. */
/* #define XPLM200 */

#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMUtilities.h"

#include "iX_Yoke_Network.h"

/*
typedef struct {
    XPLMDataRef dataref;
    int is_specified;
    float value;
} DataRefSetting;


// The network loop will write to the first, then atomically swap it with the handoff set when it's done.  When the flight loop is ready to read settings, it'll atomically swap the handoff set with the read set.  If I'm not mistaken, I think this ensures a lock-free but consistent state.
// server loop -> writeSettings <--> handoffSettings <--> readSettings -> flight loop.
DataRefSetting writeSettings[kNumTags];
DataRefSetting handoffSettings[kNumTags];
DataRefSettings readSettings[kNumTags];
*/

XPLMDataRef gOverrideRef = NULL;
XPLMDataRef gPitchRef = NULL;
XPLMDataRef gRollRef = NULL;
XPLMDataRef gYawRef = NULL;
XPLMDataRef gThrottleOverrideRef = NULL;
XPLMDataRef gThrottleRef = NULL;
XPLMDataRef gPropRef = NULL;
XPLMDataRef gFlapRef = NULL;



pthread_t server_thread = NULL;
void *server_loop(void *arg);


float flight_loop_callback(float inElapsedSinceLastCall,    
                           float inElapsedTimeSinceLastFlightLoop,    
                           int inCounter,    
                           void *inRefcon); 


float current_pitch = 0.0f;
float current_roll = 0.0f;
float current_yaw = 0.0f;
float current_throttle = 0.0f; //[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float current_prop = 0.0f; //[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float current_flap = 0.0f;
char *server_msg = NULL;



void debug(char *str)
{
    char cat[256] = "iX-Yoke: ";
    strncat(cat, str, 256 - strlen(cat) - 2);
    strcat(cat, "\n");
    XPLMDebugString(cat);
}


/*============================== REQUIRED METHODS ==============================*/

/*
 * XPluginStart
 */
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
	/* Initialize plugin registration and description */
	strcpy(outName, "iX-Yoke-plugin");
	strcpy(outSig, "com.danieldickison.iX-Yoke-plugin");
	strcpy(outDesc, "Lets the iPhone iX-Yoke app control X-Plane as a remote yoke/joystick.");
	
	gOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick");
	gPitchRef = XPLMFindDataRef("sim/joystick/yolk_pitch_ratio");
	gRollRef = XPLMFindDataRef("sim/joystick/yolk_roll_ratio");
	gYawRef = XPLMFindDataRef("sim/joystick/yolk_heading_ratio");
    gThrottleOverrideRef = XPLMFindDataRef("sim/operation/override/override_throttles");
    //gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");
    gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
    gFlapRef = XPLMFindDataRef("sim/flightmodel/controls/flaprqst");
    gPropRef = XPLMFindDataRef("sim/flightmodel/engine/POINT_pitch_deg");
    
    XPLMRegisterFlightLoopCallback(flight_loop_callback,
                                   1.0, // Start in a second...
                                   NULL);
    
	return 1;
}


/*
 * XPluginStop
 */
PLUGIN_API void	XPluginStop(void)
{
	XPLMUnregisterFlightLoopCallback(flight_loop_callback, NULL);
}


/*
 * XPluginEnable
 */
PLUGIN_API int XPluginEnable(void)
{
    debug("Starting server");
    pthread_create(&server_thread, NULL, server_loop, NULL);
	return 1;
}


/*
 * XPluginDisable
 */
PLUGIN_API void XPluginDisable(void)
{
    XPLMSetDatai(gOverrideRef, 0);
    XPLMSetDatai(gThrottleOverrideRef, 0);
    
    debug("Stopping server");
    
    // Send a kill tag to local server port.
    int sock;
    struct sockaddr_in sa;
    int bytes_sent;
    int buffer_length = 1;
    SInt8 buffer[1];
    buffer[0] = kServerKillTag;
    
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sock) /* if socket failed to initialize, exit */
    {
        debug("Error creating socket to send server kill message");
        goto server_kill_end;
    }
    
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(0x7F000001);
    sa.sin_port = htons(kServerPort);
    
    bytes_sent = sendto(sock, buffer, buffer_length, 0, (struct sockaddr*)&sa, sizeof (struct sockaddr_in));
    if (bytes_sent < 0)
    {
        debug(strerror(errno));
        goto server_kill_end;
    }
    
server_kill_end:
    close(sock);
    // Should we try to kill the server thread by another means?
}


/*
 * XPluginReceiveMessage
 */
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam)
{
}



/****** Callbacks *******/

float flight_loop_callback(float inElapsedSinceLastCall,
                           float inElapsedTimeSinceLastFlightLoop,
                           int inCounter,
                           void *inRefcon)
{
    /*
    if (inCounter % 60 == 0)
    {
        char str[64];
        sprintf(str, "throttle[0]: %f", current_throttle);
        debug(str);
    }
     */
    
    XPLMSetDatai(gOverrideRef, 1);
    XPLMSetDataf(gPitchRef, current_pitch);
    XPLMSetDataf(gRollRef, current_roll);
    XPLMSetDataf(gYawRef, current_yaw);
    
    //XPLMSetDatai(gThrottleOverrideRef, 1);
    // commented out throttle to experiment with helicopters that use FADEC..
    XPLMSetDatavf(gThrottleRef, &current_throttle, 0, 1);
    XPLMSetDatavf(gPropRef, &current_prop, 0, 1);
    //XPLMSetDataf(gFlapRef, current_flap); //No flap UI yet.
    if (server_msg != NULL)
    {
        debug(server_msg);
        server_msg = NULL;
    }
    return 0.05; //20Hz -- is this ok?
}



/****** UDP Server ******/

void *server_loop(void *arg)
{
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in addr; 
    UInt8 buffer[kPacketSizeLimit];
    size_t recv_size;
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(kServerPort);
    
    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr)))
    {
        server_msg = "Unable to bind server socket.";
        goto stop_server;
    }
    
    server_msg = "Starting server loop.";
    
    for (;;) 
    {
        recv_size = recvfrom(sock, (void *)buffer, kPacketSizeLimit, 0, NULL, NULL);
        if (recv_size < 0)
        {
            server_msg = strerror(errno);
            goto stop_server;
        }
        /*
        char debugstr[64];
        sprintf(debugstr, "Recv throttle %f", throt);
        server_msg = debugstr;
         */
        
        int i = 0;
        while (i < recv_size)
        {
            UInt8 tag = ix_get_tag(buffer, &i);
            switch (tag)
            {
                case kPacketEndTag:
                    i = 0x7fff; // Force exit the loop.
                    break;
                case kServerKillTag:
                    server_msg = "Server kill received";
                    goto stop_server;
                case kPitchTag:
                    current_pitch = ix_get_ratio(buffer, &i);
                    break;
                case kRollTag:
                    current_roll = ix_get_ratio(buffer, &i);
                    break;
                case kYawTag:
                    current_yaw = ix_get_ratio(buffer, &i);
                    break;
                case kThrottleTag:
                {
                    current_throttle = ix_get_ratio(buffer, &i);
                    /* Why doesn't this work?
                    for (int j = 0; j < 8; j++)
                    {
                        current_throttle[i] = throt;
                    }*/
                    break;
                }
                case kPropTag:
                {
                    current_prop = 15.0f * ix_get_ratio(buffer, &i);
                    /*
                    for (int j = 0; j < 8; j++)
                    {
                        current_prop[i] = prop;
                    }
                     */
                    break;
                }
                case kFlapTag:
                    current_flap = ix_get_ratio(buffer, &i);
                    break;
            }
        }
    }
    
stop_server:
    close(sock);
    return NULL;
}
