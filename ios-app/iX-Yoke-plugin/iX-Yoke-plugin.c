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
#include "XPWidgetDefs.h"
#include "XPWidgets.h"
#include "XPWidgetUtils.h"
#include "XPStandardWidgets.h"
#include "XPWidgetsEx.h"
#include "XPLMMenus.h"


#include "iX_Yoke_Network.h"


XPLMDataRef gOverrideRef = NULL;
XPLMDataRef gPitchRef = NULL;
XPLMDataRef gRollRef = NULL;
XPLMDataRef gYawRef = NULL;
XPLMDataRef gThrottleOverrideRef = NULL;
XPLMDataRef gThrottleRef = NULL;
XPLMDataRef gPropSpeedRef = NULL;
XPLMDataRef gPropRef = NULL;
XPLMDataRef gFlapRef = NULL;



pthread_t server_thread = NULL;
void *server_loop(void *arg);


float flight_loop_callback(float inElapsedSinceLastCall,    
                           float inElapsedTimeSinceLastFlightLoop,    
                           int inCounter,    
                           void *inRefcon); 


float tilt_x = 0.0f;
float tilt_y = 0.0f;
float touch_x = 0.0f;
float touch_y = 0.0f;

char *server_msg = NULL;

XPWidgetID window_id = 0;
XPWidgetID touch_y_popup_id = 0;
int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber);
int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

void menu_callback(void *menuRef, void *itemRef);

const char *axis_choices = "Throttle;Prop Pitch;Prop Speed";
enum {
    kAxisControlThrottle = 0,
    kAxisControlPropPitch,
    kAxisControlPropSpeed
};

int touch_y_control = kAxisControlThrottle;



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
	
    // Find all the datarefs.
    debug("Finding datarefs...");
	gOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick");
	gPitchRef = XPLMFindDataRef("sim/joystick/yolk_pitch_ratio");
	gRollRef = XPLMFindDataRef("sim/joystick/yolk_roll_ratio");
	gYawRef = XPLMFindDataRef("sim/joystick/yolk_heading_ratio");
    gThrottleOverrideRef = XPLMFindDataRef("sim/operation/override/override_throttles");
    //gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");
    gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
    gPropSpeedRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_prop");
    gFlapRef = XPLMFindDataRef("sim/flightmodel/controls/flaprqst");
    gPropRef = XPLMFindDataRef("sim/flightmodel/engine/POINT_pitch_deg");
    
    // Add menu for showing config window.
    debug("Adding menu...");
    XPLMMenuID pluginsMenu = XPLMFindPluginsMenu();
    int subMenuItem = XPLMAppendMenuItem(pluginsMenu, "iX-Yoke", NULL, 1);
    XPLMMenuID ixYokeMenu = XPLMCreateMenu("iX-Yoke", pluginsMenu, subMenuItem, menu_callback, NULL);
    XPLMAppendMenuItem(ixYokeMenu, "Setup Window", NULL, 0);
    
    // Register for timed callbacks.
    debug("Registering callback...");
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
    if (window_id != 0)
    {
        XPDestroyWidget(window_id, 1);
    }
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
    XPLMSetDataf(gRollRef, tilt_x);
    XPLMSetDataf(gPitchRef, tilt_y);
    XPLMSetDataf(gYawRef, touch_x);
    
    switch (touch_y_control)
    {
        case kAxisControlThrottle:
        {
            //XPLMSetDatai(gThrottleOverrideRef, 1);
            float throt[8];
            for (int i = 0; i < 8; i++)
                throt[i] = 0.5f*touch_y + 0.5;
            XPLMSetDatavf(gThrottleRef, throt, 0, 8);
            break;
        }
        case kAxisControlPropPitch:
        {
            float prop_deg[8];
            for (int i = 0; i < 8; i++)
                prop_deg[i] = 4.5 - 5.0f * touch_y; // range = [-0.5, 9.5]
            XPLMSetDatavf(gPropRef, prop_deg, 0, 8);
            break;
        }
        case kAxisControlPropSpeed:
        {
            float rad_per_sec[8];
            for (int i = 0; i < 8; i++)
                rad_per_sec[i] = 1046.66f * (0.5f*touch_y+0.5f); // range = [0, 10000rpm]
            XPLMSetDatavf(gPropSpeedRef, rad_per_sec, 0, 8);
            break;
        }
    }
    if (server_msg != NULL)
    {
        debug(server_msg);
        server_msg = NULL;
    }
    return 0.05; //20Hz -- is this ok?
}


int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber)
{
    if (inMessage != xpMessage_ListBoxItemSelected)
        return 0;
    
    if (inWidget == touch_y_popup_id)
    {
        debug("Selected touch y axis control");
        touch_y_control = inItemNumber;
    }
    return 1;
}


void menu_callback(void *menuRef, void *itemRef)
{
    if (window_id == 0)
    {
        // Create config window.
        debug("Creating config window...");
        
        int x1=200, y1=400, x2=500, y2=200;
        window_id = XPCreateWidget(x1, y1, x2, y2, 0, "iX-Yoke", 1, NULL, xpWidgetClass_MainWindow);
        XPSetWidgetProperty(window_id, xpProperty_MainWindowHasCloseBoxes, 1);
        XPAddWidgetCallback(window_id, window_callback);
        
        x1 += 10; y1 += 10; x2 -= 10; y2 -= 30;
        XPWidgetID subwin = XPCreateWidget(x1, y1, x2, y2, 1, "", 0, window_id, xpWidgetClass_SubWindow);
        XPSetWidgetProperty(subwin, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
        XPAddWidgetCallback(subwin, XPUFixedLayout);
        
        x1 += 10; y1 += 10; x2 -= 10; y2 = y1-35;
        touch_y_popup_id = XPCreatePopup(x1, y1, x2, y2, 1, axis_choices, subwin);
        XPAddWidgetCallback(touch_y_popup_id, axis_popup_callback);
    }
    
    debug("Showing config window");
    XPShowWidget(window_id);
}


int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if (inMessage == xpMessage_CloseButtonPushed)
	{
        if (window_id != 0)
        {
            XPHideWidget(window_id);
        }
        return 1;
    }
    
    return 0;
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
            if (tag == kServerKillTag)
            {
                server_msg = "Server kill received";
                goto stop_server;
            }
            else if (tag == kProtocolVersion1Tag)
            {
                tilt_x = ix_get_ratio(buffer, &i);
                tilt_y = ix_get_ratio(buffer, &i);
                touch_x = ix_get_ratio(buffer, &i);
                touch_y = ix_get_ratio(buffer, &i);
            }
            // else ignore packet
        }
    }
    
stop_server:
    close(sock);
    return NULL;
}
