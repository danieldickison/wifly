/*
 *  Preferences.c
 *  iX-Yoke-plugin
 *
 *  Created by Daniel Dickison on 5/11/09.
 *  Copyright 2009 Daniel_Dickison. All rights reserved.
 *
 */

#include "iX-Yoke-plugin.h"


#define PREFS_VERSION 1


typedef struct {
    iXControlType type;
    float min;
    float max;
} iXPresetAxis;

typedef struct {
    char name[64];
    iXPresetAxis axes[4];
} iXPreset;


static int num_presets = 0;
static iXPreset presets[32];

static const int num_readonly_presets = 3;
static const iXPreset readonly_presets[3] = {
    {
        "Stick and Throttle (read-only)",
        {
            {kAxisControlRoll, -1.0, 1.0},
            {kAxisControlPitch, -1.0, 1.0},
            {kAxisControlYaw, -0.75, 0.75},
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
            {kAxisControlPropPitch, -1.0, 10.0}
        }
    }
};


typedef enum {
    kPrefTypeInt,
    kPrefTypeString
} iXPrefType;

typedef struct {
    const char name[32];
    iXPrefType type;
    void *value;
} iXPref;

static const int num_prefs = 3;

static iXPref preferences[3] = {
    {"current-preset", kPrefTypeInt, (void *)0},
    {"auto-pause", kPrefTypeInt, (void *)1},
    {"auto-resume", kPrefTypeInt, (void *)0}
};

const char * const kPrefCurrentPreset = preferences[0].name;
const char * const kPrefAutoPause = preferences[1].name;
const char * const kPrefAutoResume = preferences[2].name;

iXPref * get_pref(const char *inPrefName);




FILE *get_prefs_file(char *mode)
{
    char path[512];
    XPLMGetSystemPath(path);
    strcat(path, "Resources");
    strcat(path, XPLMGetDirectorySeparator());
    strcat(path, "preferences");
    strcat(path, XPLMGetDirectorySeparator());
    strcat(path, "Wi-Fly.prf");
#if APL
    MacToUnixPath(path, path, 512);
#endif
    
    return fopen(path, mode);
}


void load_prefs()
{
    debug("Loading prefs...");
    
    FILE *f = get_prefs_file("r");
    if (!f) goto fail;
    
    int version;
    fscanf(f, "prefs-version = %d\n", &version);
    if (version != PREFS_VERSION) goto fail;
    
    char buffer[256];
    int process_presets = 0;
    while (fgets(buffer, 256, f))
    {
        // Skip blank line.
        if (strlen(buffer) <= 1) continue;
        
        // Start of presets section.
        if (strstr(buffer, "[PRESETS]") == buffer)
        {
            debug("Loading presets...");
            process_presets = 1;
            continue;
        }
        
        // Split buffer into name and data by the equals sign.
        char *name_end = strstr(buffer, " = ");
        if (name_end == NULL) continue;
        
        name_end[0] = '\0';
        char *name = buffer;
        char *data = name_end + 3;

        if (process_presets)
        {
            // TODO
        }
        else
        {
            iXPref *pref = get_pref(name);
            if (pref == NULL)
            {
                debug("Unknown pref:");
                debug(name);
                debug(data);
                continue;
            }
            if (pref->type == kPrefTypeInt)
            {
                pref->value = (void *)strtol(data, NULL, 10);
            }
            else if (pref->type == kPrefTypeString)
            {
                // TODO
            }
        }
    }
    set_current_preset(get_pref_int(kPrefCurrentPreset));
    fclose(f);
    return;
    
fail:
    debug("load_prefs failed");
    set_current_preset(0);
}


void save_prefs()
{
    debug("Saving prefs...");
    
    FILE *f = get_prefs_file("w");
    if (!f) return;
    
    fprintf(f, "prefs-version = %d\n", PREFS_VERSION);
    
    for (int i = 0; i < num_prefs; i++)
    {
        if (preferences[i].type == kPrefTypeInt)
        {
            fprintf(f, "%s = %d\n", preferences[i].name, (int)preferences[i].value);
        }
        else if (preferences[i].type == kPrefTypeString)
        {
            fprintf(f, "%s = %s\n", preferences[i].name, (char *)preferences[i].value);
        }
    }
    
    fwrite("\n[PRESETS]\n\n", sizeof(char), 12, f);
    for (int i = 0; i < num_presets; i++)
    {
        fprintf(f, "%s = ", presets[i].name);
        for (int j = 0; j < kNumAxes; j++)
        {
            fprintf(f, "(%d %f %f) ",
                    presets[i].axes[j].type,
                    (double)presets[i].axes[j].min,
                    (double)presets[i].axes[j].max);
            fwrite("\n", sizeof(char), 1, f);
        }
    }
    
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
    return get_pref_int(kPrefCurrentPreset);
}


void set_current_preset(int i)
{
    set_pref_int(kPrefCurrentPreset, i);
    if (i >= 0 && i < (num_readonly_presets + num_presets))
    {
        iXPreset preset;
        if (i < num_readonly_presets)
            preset = readonly_presets[i];
        else
            preset = presets[i - num_readonly_presets];
        
        for (int j = 0; j < kNumAxes; j++)
        {
            iXControlAxisRef control = get_axis((iXControlAxisID)j);
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
        iXControlAxisRef control = get_axis((iXControlAxisID)i);
        new_preset.axes[i].type = control->type;
        new_preset.axes[i].min = control->min;
        new_preset.axes[i].max = control->max;
    }
    
    // On overflow, just replace the last preset.  Hacky, but I think good enough for now.
    if (num_presets == 32) num_presets--;
    
    presets[num_presets] = new_preset;
    set_pref_int(kPrefCurrentPreset, num_readonly_presets + num_presets);
    num_presets++;
    save_prefs();
}


void delete_preset(int i)
{
    if (i < num_readonly_presets) return;
    
    num_presets--;
    for (int j = (i - num_readonly_presets); j < num_presets; j++)
    {
        presets[j] = presets[j+1];
    }
    
    if (i == get_pref_int(kPrefCurrentPreset))
    {
        set_pref_int(kPrefCurrentPreset, -1);
    }
    save_prefs();
}



iXPref * get_pref(const char *inPrefName)
{
    for (int i = 0; i < num_prefs; i++)
    {
        if (strcmp(preferences[i].name, inPrefName) == 0)
        {
            return &preferences[i];
        }
    }
    return NULL;
}

int get_pref_int(const char *inPrefName)
{
    iXPref *pref = get_pref(inPrefName);
    if (pref)
    {
        return (int)pref->value;
    }
    return 0;
}

void set_pref_int(const char *inPrefName, int val)
{
    iXPref *pref = get_pref(inPrefName);
    if (pref)
    {
        pref->value = (void *)val;
    }
}
