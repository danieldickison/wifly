/*
 *  iX-Yoke-plugin.c
 *  Wi-Fly-plugin
 *
 *  Created by Daniel Dickison on 5/11/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 * 
 *  This file is part of Wi-Fly-plugin.
 *  
 *  Wi-Fly-plugin is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  Wi-Fly-plugin is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with Wi-Fly-plugin.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Uncomment the following line ONLY if you NEED v2 ONLY APIs. */
/* #define XPLM200 */

#include "iX-Yoke-plugin.h"



// Callbacks

long previous_packet_time = -1;
int connected = 1;
float flight_loop_callback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon); 
void menu_callback(void *menuRef, void *itemRef);



// Axes

iXControlAxis _axes[4] = {
    {kAxisControlRoll, 0.5f, 0.5f, -1.0f, 1.0f, "Tilt X",0,0,0,0},
    {kAxisControlPitch, 0.5f, 0.5f, -1.0f, 1.0f, "Tilt Y",0,0,0,0},
    {kAxisControlYaw, 0.5f, 0.5f, -1.0f, 1.0f, "Touch X",0,0,0,0},
    {kAxisControlThrottle, 0.0f, 0.0f, 0.0f, 1.0f, "Touch Y",0,0,0,0}
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
XPLMDataRef gSpdbrkRef = NULL;
XPLMDataRef gNoseSteerRef = NULL;
XPLMDataRef gPausedRef = NULL;


void apply_control_value(iXControlAxisRef control);


void iXDebug(char *str)
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
    CFRelease(inStr);
    
    CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    CFRelease(url);
    
    int success = CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle);
    CFRelease(outStr);
    return (success ? 0 : -1);
}
#endif




/*============================== REQUIRED METHODS ==============================*/

/*
 * XPluginStart
 */
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
	/* Initialize plugin registration and description */
	strcpy(outName, "Wi-Fly-plugin");
	strcpy(outSig, "com.danieldickison.wi-fly-plugin");
	strcpy(outDesc, "Lets the Wi-Fly iPhone app control X-Plane as a remote yoke/joystick.");
	
    // Find all the datarefs.
    iXDebug("Finding datarefs...");
	gStickOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick");
	gPitchOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick_pitch");
	gRollOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick_roll");
	gYawOverrideRef = XPLMFindDataRef("sim/operation/override/override_joystick_heading");
	gPitchRef = XPLMFindDataRef("sim/joystick/yolk_pitch_ratio");
	gRollRef = XPLMFindDataRef("sim/joystick/yolk_roll_ratio");
	gYawRef = XPLMFindDataRef("sim/joystick/yolk_heading_ratio");
    gThrottleOverrideRef = XPLMFindDataRef("sim/operation/override/override_throttles");
    //gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");
    gThrottleRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
    gPropSpeedRef = XPLMFindDataRef("sim/flightmodel/engine/ENGN_prop");
    gFlapRef = XPLMFindDataRef("sim/flightmodel/controls/flaprqst");
    gSpdbrkRef = XPLMFindDataRef("sim/flightmodel/controls/sbrkrqst");
    gPropRef = XPLMFindDataRef("sim/flightmodel/engine/POINT_pitch_deg");
    gNoseSteerRef = XPLMFindDataRef("sim/cockpit2/controls/nosewheel_steer_on");
    gPausedRef = XPLMFindDataRef("sim/time/paused");
    
    // Add menu for showing config window.
    iXDebug("Adding menu...");
    XPLMMenuID pluginsMenu = XPLMFindPluginsMenu();
    int subMenuItem = XPLMAppendMenuItem(pluginsMenu, "Wi-Fly Remote", NULL, 1);
    XPLMMenuID ixYokeMenu = XPLMCreateMenu("Wi-Fly Remote", pluginsMenu, subMenuItem, menu_callback, NULL);
    XPLMAppendMenuItem(ixYokeMenu, "Setup Window", NULL, 0);
    
	return 1;
}


/*
 * XPluginStop
 */
PLUGIN_API void	XPluginStop(void)
{
}


/*
 * XPluginEnable
 */
PLUGIN_API int XPluginEnable(void)
{
    iXDebug("Starting server");
    previous_packet_time = -1;
    connected = 1;
    start_server();
    load_prefs();
    
    // Register for timed callbacks.
    iXDebug("Registering callback...");
    XPLMRegisterFlightLoopCallback(flight_loop_callback,
                                   1.0, // Start in a second...
                                   NULL);
	return 1;
}


/*
 * XPluginDisable
 */
PLUGIN_API void XPluginDisable(void)
{
	XPLMUnregisterFlightLoopCallback(flight_loop_callback, NULL);
    destroy_window();
    
    XPLMSetDatai(gStickOverrideRef, 0);
    XPLMSetDatai(gPitchOverrideRef, 0);
    XPLMSetDatai(gYawOverrideRef, 0);
    XPLMSetDatai(gRollOverrideRef, 0);
    XPLMSetDatai(gThrottleOverrideRef, 0);
    
    iXDebug("Stopping server");
    stop_server();
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
    long packet_time = get_last_packet_time();
    
    // Pause and show window if no update in a second.
    if (previous_packet_time == packet_time)
    {
        long current_time = get_ms_time();
        if (current_time - previous_packet_time > 1000 &&
            connected)
        {
            
            connected = 0;
            if (get_pref_int(kPrefAutoPause))
            {
                set_pause_state(1);
                show_window();
            }
        }
    }
    else
    {
        if (!connected &&
            get_pref_int(kPrefAutoResume))
        {
            set_pause_state(0);
            destroy_window();
        }
        for (int i = 0; i < kNumAxes; i++)
        {
            apply_control_value(get_axis((iXControlAxisID)i));
        }
        
        previous_packet_time = packet_time;
        connected = 1;
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
    // Only apply changed axes to avoid conflicting with autopilot (autothrottle, in particular).
    if (control->value == control->prev_value) return;
    
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
            copy_float_to_array(value, eight_floats, 8);
            XPLMSetDatavf(gThrottleRef, eight_floats, 0, 8);
            break;
        case kAxisControlPropPitch:
            copy_float_to_array(value, eight_floats, 8);
            XPLMSetDatavf(gPropRef, eight_floats, 0, 8);
            break;
        case kAxisControlSpeedBrake:
            XPLMSetDataf(gSpdbrkRef, value);
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
    // Only set this when using ENGN_thro_use, not for ENGN_thro
    //XPLMSetDatai(gThrottleOverrideRef, override_throttle);
}



void menu_callback(void *menuRef, void *itemRef)
{
    show_window();
}



