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



// Callbacks

float flight_loop_callback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon); 
void menu_callback(void *menuRef, void *itemRef);



// Axes

iXControlAxis _axes[4] = {
    {kAxisControlRoll, 0.5f, -1.0f, 1.0f, "Tilt X",0,0,0,0},
    {kAxisControlPitch, 0.5f, -1.0f, 1.0f, "Tilt Y",0,0,0,0},
    {kAxisControlYaw, 0.5f, -1.0f, 1.0f, "Touch X",0,0,0,0},
    {kAxisControlThrottle, 0.0f, 0.0f, 1.0f, "Touch Y",0,0,0,0}
};

iXControlAxisRef get_axis(iXControlAxisID axis_id)
{
    return &_axes[axis_id];
}


// Datarefs

XPLMDataRef gStickOverrideRef = NULL;
XPLMDataRef gPitchOverrideRef = NULL;
XPLMDataRef gPitchRef = NULL;
XPLMDataRef gRollOverrideRef = NULL;
XPLMDataRef gRollRef = NULL;
XPLMDataRef gYawOverrideRef = NULL;
XPLMDataRef gYawRef = NULL;
XPLMDataRef gThrottleOverrideRef = NULL;
XPLMDataRef gThrottleRef = NULL;
XPLMDataRef gPropSpeedRef = NULL;
XPLMDataRef gPropRef = NULL;
XPLMDataRef gFlapRef = NULL;
XPLMDataRef gNoseSteerRef = NULL;
XPLMDataRef gPausedRef = NULL;



// Server etc.
#if IBM
HANDLE server_thread = NULL;
void server_loop(LPVOID pParameter);
#else
pthread_t server_thread = NULL;
void *server_loop(void* pParameter);
#endif

char *server_msg = NULL;
char *server_ip = NULL;

int connected = 0;


void apply_control_value(iXControlAxisRef control);


void debug(char *str)
{
    char cat[256] = "iX-Yoke: ";
    strlcat(cat, str, 256-1);
    strcat(cat, "\n");
    XPLMDebugString(cat);
}


long get_ms_time()
{
    static long epoch_sec = -1;
    struct timeval time;
    gettimeofday(&time, NULL);
    if (epoch_sec == -1)
    {
        epoch_sec = time.tv_sec;
    }
    long result = 1000 * (time.tv_sec - epoch_sec);
    result += (time.tv_usec / 1000);
    return result;
}


void set_pause_state(int state)
{
    int paused = XPLMGetDatai(gPausedRef);
    if ((paused && !state) || (!paused && state))
    {
        XPLMCommandKeyStroke(xplm_key_pause);
    }
}


int currently_connected()
{
    return connected;
}



#if APL
#include <Carbon/Carbon.h>
int MacToUnixPath(const char * inPath, char * outPath, int outPathMaxLen)
{
    CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath, kCFStringEncodingMacRoman);
    if (inStr == NULL) return -1;
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
    CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle)) return -1;
    CFRelease(outStr);
    CFRelease(url);
    CFRelease(inStr);
    return 0;
}
#endif




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
	gStickOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick");
	gPitchOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick_pitch");
	gRollOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick_roll");
	gYawOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick_heading");
	gPitchRef = XPLMFindDataRef("sim/joystick/yolk_pitch_ratio");
	gRollRef = XPLMFindDataRef("sim/joystick/yolk_roll_ratio");
	gYawRef = XPLMFindDataRef("sim/joystick/yolk_heading_ratio");
    gThrottleOverrideRef = XPLMFindDataRef("sim/operation/override/override_throttles");
    gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");
    //gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
    gPropSpeedRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_prop");
    gFlapRef = XPLMFindDataRef("sim/flightmodel/controls/flaprqst");
    gPropRef = XPLMFindDataRef("sim/flightmodel/engine/POINT_pitch_deg");
    gNoseSteerRef = XPLMFindDataRef("sim/cockpit2/controls/nosewheel_steer_on");
    gPausedRef = XPLMFindDataRef("sim/time/paused");
    
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
    destroy_window();
}


/*
 * XPluginEnable
 */
PLUGIN_API int XPluginEnable(void)
{
    debug("Starting server");
#if IBM
	    WSADATA wsaData;
		WSAStartup(MAKEWORD(1, 1), &wsaData);
		_beginthread(server_loop, 0, NULL);
#else
	pthread_create(&server_thread, NULL, server_loop, NULL);
#endif
    
    load_prefs();
	return 1;
}


/*
 * XPluginDisable
 */
PLUGIN_API void XPluginDisable(void)
{
    XPLMSetDatai(gStickOverrideRef, 0);
    XPLMSetDatai(gPitchOverrideRef, 0);
    XPLMSetDatai(gYawOverrideRef, 0);
    XPLMSetDatai(gRollOverrideRef, 0);
    XPLMSetDatai(gThrottleOverrideRef, 0);
    
    debug("Stopping server");
    
    // Send a kill tag to local server port.
    int sock;
    struct sockaddr_in sa;
    int bytes_sent;
    int buffer_length = 1;
    int8_t buffer[1];
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
    closesocket(sock);
	// Should we try to kill the server thread by another means?
#if IBM
    WSACleanup();
#endif
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
    static long previous_packet_time = -1;
    long packet_time = get_last_packet_time();
    
    // Pause and show window if no update in a second.
    if (previous_packet_time == packet_time)
    {
        long current_time = get_ms_time();
        if (current_time - previous_packet_time > 1000 &&
            connected)
        {
            set_pause_state(1);
            connected = 0;
            show_window();
        }
    }
    else
    {
        for (int i = 0; i < kNumAxes; i++)
        {
            apply_control_value(get_axis((iXControlAxisID)i));
        }
        
        previous_packet_time = packet_time;
        connected = 1;
    }
    
    if (server_msg != NULL)
    {
        debug(server_msg);
        server_msg = NULL;
    }
    
    update_window();    
    return 0.05; //20Hz -- is this ok?
}


void copy_float_to_array(float x, float *arr, int n)
{
    for (int i = 0; i < n; i++)
        arr[i] = x;
}


void apply_control_value(iXControlAxisRef control)
{
    float eight_floats[8];
    float value = control->min + control->value * (control->max - control->min);
    switch (control->type)
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
        case kAxisControlRollAndYaw:
            XPLMSetDataf(gRollRef, value);
            XPLMSetDataf(gYawRef, value*0.2);
            break;
        case kAxisControlThrottle:
            //XPLMSetDatai(gThrottleOverrideRef, 1);
            copy_float_to_array(value, eight_floats, 8);
            XPLMSetDatavf(gThrottleRef, eight_floats, 0, 8);
            break;
        case kAxisControlPropPitch:
            copy_float_to_array(value, eight_floats, 8);
            XPLMSetDatavf(gPropRef, eight_floats, 0, 8);
            break;
        case kAxisControlOff:
            // Do nothing.
            break;
    }
}


void update_overrides()
{
    int override_throttle = 0;
    int override_pitch = 0;
    int override_roll = 0;
    int override_yaw = 0;
    for (int i = 0; i < kNumAxes; i++)
    {
        iXControlAxisRef control = get_axis((iXControlAxisID)i);
        override_throttle |= (control->type == kAxisControlThrottle);
        override_roll |= (control->type == kAxisControlRoll);
        override_roll |= (control->type == kAxisControlRollAndYaw);
        override_yaw |= (control->type == kAxisControlYaw);
        override_yaw |= (control->type == kAxisControlRollAndYaw);
        override_pitch |= (control->type == kAxisControlPitch);
    }
    XPLMSetDatai(gStickOverrideRef, override_roll && override_pitch && override_yaw);
    XPLMSetDatai(gRollOverrideRef, override_roll);
    XPLMSetDatai(gYawOverrideRef, override_yaw);
    XPLMSetDatai(gPitchOverrideRef, override_pitch);
    XPLMSetDatai(gThrottleOverrideRef, override_throttle);
}



void menu_callback(void *menuRef, void *itemRef)
{
    show_window();
}



