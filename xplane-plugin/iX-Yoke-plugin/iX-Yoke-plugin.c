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

#include "iX-Yoke-plugin.h"

XPLMDataRef gOverrideRef = NULL;
XPLMDataRef gPitchRef = NULL;
XPLMDataRef gRollRef = NULL;
XPLMDataRef gYawRef = NULL;
XPLMDataRef gThrottleOverrideRef = NULL;
XPLMDataRef gThrottleRef = NULL;
XPLMDataRef gPropSpeedRef = NULL;
XPLMDataRef gPropRef = NULL;
XPLMDataRef gFlapRef = NULL;


XPWidgetID window_id = 0;
XPWidgetID touch_y_popup_id = 0;


const char *axis_choices = "Pitch;Roll;Yaw;Throttle;Prop Pitch";


iXControlAxis tilt_x = {kAxisControlRoll, 0.0f, -1.0f, 1.0f};
iXControlAxis tilt_y = {kAxisControlPitch, 0.0f, -1.0f, 1.0f};
iXControlAxis touch_x = {kAxisControlYaw, 0.0f, -1.0f, 1.0f};
iXControlAxis touch_y = {kAxisControlThrottle, -1.0f, 0.0f, 1.0f};

pthread_t server_thread = NULL;
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
    
    //XPLMSetDatai(gOverrideRef, 1);
    apply_control_value(tilt_x);
    apply_control_value(tilt_y);
    apply_control_value(touch_x);
    apply_control_value(touch_y);

    if (server_msg != NULL)
    {
        debug(server_msg);
        server_msg = NULL;
    }
    return 0.05; //20Hz -- is this ok?
}


void copy_float_to_array(float x, float *arr, int n)
{
    for (int i = 0; i < n; i++)
        arr[i] = x;
}


void apply_control_value(iXControlAxis control)
{
    float eight_floats[8];
    float value = control.min + control.value * (control.max - control.min);
    switch (control.type)
    {
        case kAxisControlPitch:
            XPLMSetDataf(gPitchRef, value);
            break;
        case kAxisControlRoll:
            XPLMSetDataf(gRollRef, value);
            break;
        case kAxisControlYaw:
            XPLMSetDataf(gYawRef, value);
            break;
        case kAxisControlThrottle:
            //XPLMSetDatai(gThrottleOverrideRef, 1);
            copy_float_to_array(value, eight_floats, 8);
            XPLMSetDatavf(gThrottleRef, eight_floats, 0, 8);
            break;
        case kAxisControlPropPitch:
            // range --> [-0.5, 9.5]
            copy_float_to_array(value, eight_floats, 8);
            XPLMSetDatavf(gPropRef, eight_floats, 0, 8);
            break;
    }
}


int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber)
{
    if (inMessage != xpMessage_PopupNewItemPicked)
    {
        return 0;
    }
    
    if ((void*)inPopupID == touch_y_popup_id)
    {
        debug("Selected touch y axis control");
        touch_y.type = inItemNumber;
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
        
        x1 += 10; y1 -= 30; x2 -= 10; y2 += 10;
        XPWidgetID subwin = XPCreateWidget(x1, y1, x2, y2, 1, "", 0, window_id, xpWidgetClass_SubWindow);
        XPSetWidgetProperty(subwin, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
        XPAddWidgetCallback(subwin, XPUFixedLayout);
        
        x1 += 10; y1 -= 10; x2 -= 10; y2 = y1-30;
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



