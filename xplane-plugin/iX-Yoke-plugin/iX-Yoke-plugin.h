/*
 *  iX-Yoke-plugin.h
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 5/9/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */


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


void *server_loop(void *arg);
extern pthread_t server_thread;
extern char *server_msg;



typedef enum {
    kAxisControlPitch = 0,
    kAxisControlRoll,
    kAxisControlYaw,
    kAxisControlThrottle,
    kAxisControlPropPitch
} iXControlType;


typedef struct {
    iXControlType type;
    float value;
    float min;
    float max;
} iXControlAxis;


extern iXControlAxis tilt_x;
extern iXControlAxis tilt_y;
extern iXControlAxis touch_x;
extern iXControlAxis touch_y;



void apply_control_value(iXControlAxis control);





// Callbacks

int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber);
int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
float flight_loop_callback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon); 
void menu_callback(void *menuRef, void *itemRef);


