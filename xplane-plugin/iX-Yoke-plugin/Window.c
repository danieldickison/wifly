/*
 *  Window.c
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 5/10/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */

#include "iX-Yoke-plugin.h"


int preset_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber);
int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber);
int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);
int textfield_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

XPWidgetID window_id = 0;
XPWidgetID preset_popup_id = 0;

void get_preset_menu_str(char *outStr);
void update_settings_display();


void destroy_window()
{
    if (window_id != 0)
    {
        XPDestroyWidget(window_id, 1);
        window_id = 0;
    }
}


void show_window()
{
    if (window_id == 0)
    {
        // Create config window.
        debug("Creating config window...");
        
        int x1=200, y1=500, x2=500, y2=200;
        window_id = XPCreateWidget(x1, y1, x2, y2, 0, "iX-Yoke", 1, NULL, xpWidgetClass_MainWindow);
        XPSetWidgetProperty(window_id, xpProperty_MainWindowHasCloseBoxes, 1);
        XPAddWidgetCallback(window_id, window_callback);
        
        
        // Add preset controls.
        x1 += 10; x2 -= 10; y1 -= 30; y2 += 10;
        XPCreateWidget(x1, y1, x1+50, y1-20, 1, "Preset:", 0, window_id, xpWidgetClass_Caption);
        preset_popup_id = XPCreatePopup(x1+55, y1, x2-100, y1-25, 1, "Presets", window_id);
        XPAddWidgetCallback(preset_popup_id, preset_popup_callback);
        
        
        // Add subwindow and controls for each axis.
        y1 -= 30;
        XPWidgetID subwin = XPCreateWidget(x1, y1, x2, y2, 1, "", 0, window_id, xpWidgetClass_SubWindow);
        XPSetWidgetProperty(subwin, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
        XPAddWidgetCallback(subwin, XPUFixedLayout);
        
        x1 += 10; x2 -= 10; y2 = y1;
        for (int i = 0; i < kNumAxes; i++)
        {
            y1 = y2-10; y2 = y1-20;
            iXControlAxisRef control = get_axis(i);
            
            // Axis title.
            XPCreateWidget(x1, y1, x1+100, y2, 1, control->title, 0, subwin, xpWidgetClass_Caption);
            
            // Popup
            control->popup_widget = XPCreatePopup(x1+120, y1, x2, y2, 1, axis_choices, subwin);
            XPAddWidgetCallback(control->popup_widget, axis_popup_callback);
            
            // Min, Max, progress
            y1 = y2-5; y2 = y1-20;
            control->min_widget = XPCreateWidget(x1, y1, x1+50, y2, 1, "-1", 0, subwin, xpWidgetClass_TextField);
            control->max_widget = XPCreateWidget(x2-50, y1, x2, y2, 1, "1", 0, subwin, xpWidgetClass_TextField);
            XPAddWidgetCallback(control->min_widget, textfield_callback);
            XPAddWidgetCallback(control->max_widget, textfield_callback);
            
            control->progress_widget = XPCreateWidget(x1+60, y1, x2-60, y2, 1, "", 0, subwin, xpWidgetClass_Progress);
            XPSetWidgetProperty(control->progress_widget, xpProperty_ProgressMin, 0);
            XPSetWidgetProperty(control->progress_widget, xpProperty_ProgressMax, 1024);
        }
    }
    
    update_settings_display();
    
    XPShowWidget(window_id);
}



int update_window()
{
    if (!window_id) return 0;
    
    for (int i = 0; i < kNumAxes; i++)
    {
        iXControlAxisRef control = get_axis(i);
        XPWidgetID progress = control->progress_widget;
        float value = control->value;
        long intValue = (long)(1024.0f * value);
        XPSetWidgetProperty(progress, xpProperty_ProgressPosition, intValue);
    }
    
    return 1;
}



int axis_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber)
{
    if (inMessage != xpMessage_PopupNewItemPicked)
    {
        return 0;
    }
    
    iXControlAxisRef control = NULL;
    for (int i = 0; i < kNumAxes; i++)
    {
        if (get_axis(i)->popup_widget == (void*)inPopupID)
        {
            control = get_axis(i);
            break;
        }
    }
    if (!control) return 0;
    
    if (control->type != inItemNumber)
    {
        control->type = inItemNumber;
        set_current_preset(-1);
        update_settings_display();
        update_overrides();
    }
    
    return 1;
}


int window_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if (inMessage == xpMessage_CloseButtonPushed)
	{
        destroy_window();
        return 1;
    }
    
    return 0;
}



int textfield_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
    if (inMessage == xpMsg_KeyPress)
    {
        // Only allow numeric, decimal point and minus characters.
        XPKeyState_t *key_state = (void *)inParam1;
        char key = key_state->key;
        char vkey = key_state->vkey;
        if (key == '\n' || key == '\r')
        {
            XPLoseKeyboardFocus(inWidget);
            return 1;
        }
        if ((key_state->flags & (xplm_ControlFlag | xplm_ShiftFlag | xplm_OptionAltFlag)) ||
            ((key < '0' || key > '9') && (key != '.') && (key != '-') &&
             (key != 0x7f) && (key != '\b') &&
             (vkey != XPLM_VK_LEFT) && (vkey != XPLM_VK_RIGHT) &&
             (vkey != XPLM_VK_ESCAPE)))
        {
            return 1;
        }
    }
    else if (inMessage == xpMsg_KeyLoseFocus)
    {
        XPWidgetID textfield = inWidget;
        iXControlAxisRef control = NULL;
        int isMin = 0;
        for (int i = 0; i < kNumAxes; i++)
        {
            iXControlAxisRef this_control = get_axis(i);
            if (this_control->min_widget == textfield)
            {
                isMin = 1;
                control = this_control;
                break;
            }
            else if (this_control->max_widget == textfield)
            {
                isMin = 0;
                control = this_control;
                break;
            }
        }
        if (!control) return 0;
        
        char str[32];
        XPGetWidgetDescriptor(textfield, str, 32);
        str[31] = '\0'; // Null-terminate in case user typed >31 chars.
        float value = atof(str);
        int value_changed;
        if (isMin)
        {
            value_changed = control->min != value;
            control->min = value;
        }
        else
        {
            value_changed = control->max != value;
            control->max = value;
        }
        
        if (value_changed)
        {
            set_current_preset(-1);
            update_settings_display();
        }
        
        return 1;
    }
    return 0;
}


int preset_popup_callback(XPWidgetMessage inMessage, XPWidgetID inWidget, long inPopupID, long inItemNumber)
{
    if (inMessage == xpMessage_PopupNewItemPicked)
    {
        set_current_preset(inItemNumber - 1);
        update_settings_display();
        return 1;
    }
    return 0;
}



void update_settings_display()
{
    debug(server_ip);
    
    char preset_menu_str[65*48];
    get_preset_menu_str(preset_menu_str);
    XPSetWidgetDescriptor(preset_popup_id, preset_menu_str);
    XPSetWidgetProperty(preset_popup_id, xpProperty_PopupCurrentItem, 1+current_preset());
    
    
    // Set all the popups and text fields' values.
    for (int i = 0; i < kNumAxes; i++)
    {
        iXControlAxisRef control = get_axis(i);
        
        XPSetWidgetProperty(control->popup_widget, xpProperty_PopupCurrentItem, control->type);
        
        char str[7];
        snprintf(str, 7, "%f", (double)control->min);
        XPSetWidgetDescriptor(control->min_widget, str);
        snprintf(str, 7, "%f", (double)control->max);
        XPSetWidgetDescriptor(control->max_widget, str);
    }
}


void get_preset_menu_str(char *outStr)
{
    char *names[48];
    int n = get_preset_names(names);
    strcpy(outStr, "Custom");
    for (int i = 0; i < n; i++)
    {
        strcat(outStr, ";");
        strcat(outStr, names[i]);
    }
}

