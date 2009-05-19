/*
 *  Preferences.c
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 5/11/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */

#include "iX-Yoke-plugin.h"

typedef struct {
    iXControlType type;
    float min;
    float max;
} iXPresetAxis;

typedef struct {
    char name[64];
    iXPresetAxis axes[4];
} iXPreset;


static int current_pre = -1;
static int num_presets = 0;
static iXPreset presets[32];

static const int num_readonly_presets = 3;
static const iXPreset readonly_presets[3] = {
    {
        "Stick and Throttle (read-only)",
        {
            {kAxisControlRoll, -1.0, 1.0},
            {kAxisControlPitch, -1.0, 1.0},
            {kAxisControlYaw, -1.0, 1.0},
            {kAxisControlThrottle, 0.0, 1.0}
        }
    },
    {
        "Auto-coordinate Rudder (read-only)",
        {
            {kAxisControlRollAndYaw, -1.0, 1.0},
            {kAxisControlPitch, -1.0, 1.0},
            {kAxisControlOff, -1.0, 1.0},
            {kAxisControlThrottle, 0.0, 1.0}
        }
    },
    {
        "Helicopter Stick and Collective (read-only)",
        {
            {kAxisControlRoll, -1.0, 1.0},
            {kAxisControlPitch, -1.0, 1.0},
            {kAxisControlYaw, -1.0, 1.0},
            {kAxisControlPropPitch, 10.0, -1.0}
        }
    }
};


FILE *get_prefs_file(char *mode)
{
    char path[512];
    XPLMGetSystemPath(path);
    strcat(path, "Resources");
    strcat(path, XPLMGetDirectorySeparator());
    strcat(path, "preferences");
    strcat(path, XPLMGetDirectorySeparator());
    strcat(path, "iX-Yoke.prf");
#if APL
    MacToUnixPath(path, path, 512);
#endif
    
    return fopen(path, mode);
}


void load_prefs()
{
    FILE *f = get_prefs_file("r");
    if (f)
    {
        
        //...
        fclose(f);
    }
    else
    {
        set_current_preset(0);
    }
}


void save_prefs()
{
    FILE *f = get_prefs_file("w");
    if (!f) return;
    
    //...
    fclose(f);
}


int get_preset_names(char **outNames)
{
    int i = 0;
    for (int j = 0; j < num_readonly_presets; j++)
    {
        outNames[i++] = (char*)readonly_presets[j].name;
    }
    for (int j = 0; j < num_presets; j++)
    {
        outNames[i++] = (char*)presets[j].name;
    }
    return num_presets + num_readonly_presets;
}


int current_preset()
{
    return current_pre;
}


void set_current_preset(int i)
{
    current_pre = i;
    if (i >= 0 && i < (num_readonly_presets + num_presets))
    {
        iXPreset preset;
        if (i < num_readonly_presets)
            preset = readonly_presets[i];
        else
            preset = presets[i - num_readonly_presets];
        
        for (int j = 0; j < kNumAxes; j++)
        {
            iXControlAxisRef control = get_axis(j);
            iXPresetAxis pre_axis = preset.axes[j];
            control->type = pre_axis.type;
            control->min = pre_axis.min;
            control->max = pre_axis.max;
        }
        update_overrides();
    }
}


void save_preset_as(const char *inName)
{
    iXPreset new_preset;
    strncpy(new_preset.name, inName, 64);
    new_preset.name[63] = '\0';
    for (int i = 0; i < kNumAxes; i++)
    {
        iXControlAxisRef control = get_axis(i);
        new_preset.axes[i].type = control->type;
        new_preset.axes[i].min = control->min;
        new_preset.axes[i].max = control->max;
    }
    
    // On overflow, just replace the last preset.  Hacky, but I think good enough for now.
    if (num_presets == 32) num_presets--;
    
    presets[num_presets] = new_preset;
    current_pre = num_readonly_presets + num_presets;
    num_presets++;
}


void delete_preset(int i)
{
    if (i < num_readonly_presets) return;
    
    num_presets--;
    for (int j = (i - num_readonly_presets); j < num_presets; j++)
    {
        presets[j] = presets[j+1];
    }
    
    if (i == current_pre)
    {
        current_pre = -1;
    }
}
