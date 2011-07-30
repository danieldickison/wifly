//
//  TiltController.m
//  Wi-Fly-app
//
//  Created by Daniel Dickison on 7/30/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "TiltController.h"
#import "iX_YokeAppDelegate.h"

#define kUpdateFrequency 30  // Hz
#define kFilteringFactor 0.25f


NSString * const iXTiltUpdatedNotification = @"iXTiltUpdatedNotification";

// Matrices should be an array in column-major order of floats.  outMatrix must have enough space to store the results, which will be aRows x bCols.
static void matMult(float* outMatrix, float *A, float *B, int aCols_bRows, int aRows, int bCols);
static void rotMat(float* outMat9, const float* axis3, float theta);



@implementation TiltController

@synthesize x, y, hold_x, hold_y, hold, landscape;

- (id)init
{
    if ((self = [super init]))
    {
        [UIAccelerometer sharedAccelerometer].updateInterval = (1.0 / kUpdateFrequency);
        [UIAccelerometer sharedAccelerometer].delegate = self;
        
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        landscape = [defaults boolForKey:@"landscape"];
        hold_x = [defaults floatForKey:@"tilt_hold_x"];
        hold_y = [defaults floatForKey:@"tilt_hold_y"];
        x = y = 0.5;
        
        NSArray *savedTiltMatrix = [defaults objectForKey:@"tiltTransformMatrix"];
        if ([savedTiltMatrix count] == 9)
        {
            for (int i = 0; i < 9; i++)
            {
                portraitTransformMatrix[i] = [[savedTiltMatrix objectAtIndex:i] floatValue];
            }
        }
        else
        {
            // Default transform is portrait orientation tilted forward ~45 degrees.
            portraitTransformMatrix[0] = 1.0f;
            portraitTransformMatrix[4] = 0.707f;
            portraitTransformMatrix[5] = 0.707f;
            portraitTransformMatrix[7] = -0.707f;
            portraitTransformMatrix[8] = 0.707f;
        }
    }
    return self;
}


- (void)getAccelerationVector:(float[3])outVector3
{
    outVector3[0] = acceleration[0];
    outVector3[1] = acceleration[1];
    outVector3[2] = acceleration[2];
}


- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)accel
{
    // Use a basic low-pass filter to reduce jitter.
    // See the BubbleLevel Apple example.
    acceleration[0] = (float)accel.x * kFilteringFactor + acceleration[0] * (1.0f - kFilteringFactor);
    acceleration[1] = (float)accel.y * kFilteringFactor + acceleration[1] * (1.0f - kFilteringFactor);
    acceleration[2] = (float)accel.z * kFilteringFactor + acceleration[2] * (1.0f - kFilteringFactor);
    
    if (landscape)
    {
        x = 0.5 - 0.5*acceleration[1];
        y = (acceleration[2] >= 0 ?
             1 :
             acceleration[0] > 0 ?
             0 :
             atanf(acceleration[0] / acceleration[2]) / M_PI_2);
    }
    else
    {
        // Apply the rotation matrix to center it about the z-axis.
        float rotated[3];
        matMult(rotated, portraitTransformMatrix, acceleration, 3, 3, 1);
        
        // Project to the xy plane for pitch & roll.
        x = 0.5f * (1.0f + rotated[0]);
        y = 0.5f * (1.0f - rotated[1]);
    }
    
    x = MAX(0.0f, MIN(1.0f, x));
    y = MAX(0.0f, MIN(1.0f, y));
    
    RemoteController *remote = SharedAppDelegate.remoteController;
    if (!hold)
    {
        remote.tilt_x = hold_x = x;
        remote.tilt_y = hold_y = y;
    }
    [remote send];
    [[NSNotificationCenter defaultCenter] postNotificationName:iXTiltUpdatedNotification object:self];
}



- (void)setPortraitCalibrationWithCenterVector:(float[3])cv forwardVector:(float[3])fv
{
    // The two vectors are the center orientation and the full-forward orientation.
    // We want to find the rotation matrix to rotate cv onto the z-axis (k), and fv onto the y-z plane.
    // To do this, we compose 2 rotations:
    // 1. Rotate about (cv x k) to map cv onto k.
    // 2. Rotate about k to map fv' onto the y-z plane.
    
    // Rotation 1: cv to k
    // Theta will be the angle between k and cv, found using the dot product.
    // The axis of rotation will be n, the unit normal of k and cv, found using cross product.
    
    // k dot cv = <0,0,1> dot <cx,cy,cz> = cz
    float cmag = sqrt(cv[0]*cv[0] + cv[1]*cv[1] + cv[2]*cv[2]);
    float ctheta = acosf(cv[2] / cmag);
    if (cv[1] < 0) ctheta = -ctheta;
    
    // k cross cv = <cx,cy,cz> cross <0,0,1> = <cy, -cx, 0>
    float caxis[3] = {cv[1], -cv[0], 0.0f};
    float rotation1[9];
    rotMat(rotation1, caxis, ctheta);
    
    
    // Rotation 2: fv' to the y-z plane.
    // fv' is fv transformed by rotation1.
    // Theta will be the angle between j and fv' projected to the x-y plane.
    // Axis will be k.
    
    float fvp[3]; // fv'
    matMult(fvp, rotation1, fv, 3, 3, 1);
    
    // fv' projected to x-y plane is just the x and y componenst of fv'.
    // Therefore theta = atan(fvp[x] / fvp[y])
    float ftheta = -atanf(fvp[0] / fvp[1]);
    if (fvp[1] > 0) ftheta += M_PI;
    float faxis[3] = {0.0f, 0.0f, 1.0f};
    float rotation2[9];
    rotMat(rotation2, faxis, ftheta);
    
    
    // Finally, scale up  flip the y axis.
    float scale = 1.0f / sqrt(fvp[0]*fvp[0] + fvp[1]*fvp[1]);
    float scaleFlip[9] = {scale, 0, 0, 0, -scale, 0, 0, 0, scale};
    
    
    // Compose the 2 rotations and the flip.
    float rotations[9];
    matMult(rotations, rotation2, rotation1, 3, 3, 3);
    matMult(portraitTransformMatrix, scaleFlip, rotations, 3, 3, 3);
    
    
    // Save the transform matrix in prefs.
    NSMutableArray *saveTransform = [[NSMutableArray alloc] initWithCapacity:9];
    for (int i = 0; i < 9; i++)
    {
        [saveTransform addObject:[NSNumber numberWithFloat:portraitTransformMatrix[i]]];
    }
    [[NSUserDefaults standardUserDefaults] setObject:saveTransform forKey:@"tiltTransformMatrix"];
    [saveTransform release];
    
    NSLog(@"Calibration has been set:");
    NSLog(@"Center vector: <%f, %f, %f>", cv[0], cv[1], cv[2]);
    NSLog(@"Forward vector: <%f, %f, %f>", fv[0], fv[1], fv[2]);
    NSLog(@"Rotation axis: <%f, %f, %f>", caxis[0], caxis[1], caxis[2]);
    NSLog(@"Forward vector rotated: <%f, %f, %f>", fvp[0], fvp[1], fvp[2]);
    NSLog(@"Rotation 1 angle: %f degrees", 180.0f * ctheta / M_PI);
    NSLog(@"Rotation 2 angle: %f degrees", 180.0f * ftheta / M_PI);
}


@end



void matMult(float* outMatrix, float *A, float *B, int aCols_bRows, int aRows, int bCols)
{
    for (int i = 0; i < aRows; i++)
    {
        for (int j = 0; j < bCols; j++)
        {
            float *val = outMatrix + i + j*aRows;
            *val = 0;
            
            float *aVal = A + i;
            float *bVal = B + j*aCols_bRows;
            
            for (int k = 0; k < aCols_bRows; k++)
            {
                *val += (*aVal) * (*bVal);
                aVal += aCols_bRows;
                bVal += 1;
            }
        }
    }
}


void rotMat(float* outMat9, const float* axis, float theta)
{
    float mag = sqrtf(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
    float x, y, z;
    if (mag == 0)
    {
        x = 1;
        y = z = 0;
    }
    else
    {
        x = axis[0] / mag;
        y = axis[1] / mag;
        z = axis[2] / mag;
    }
    float s = sinf(theta);
    float c = cosf(theta);
    
    // See http://en.wikipedia.org/wiki/Rotation_matrix#Axis_of_a_rotation
    outMat9[0] = (x*x)*(1-c) + c;
    outMat9[1] = (x*y)*(1-c) - (z*s); //+-
    outMat9[2] = (x*z)*(1-c) + (y*s); //+-
    outMat9[3] = (x*y)*(1-c) + (z*s); //+-
    outMat9[4] = (y*y)*(1-c) + c;
    outMat9[5] = (y*z)*(1-c) - (x*s); //+-
    outMat9[6] = (x*z)*(1-c) - (y*s); //+-
    outMat9[7] = (y*z)*(1-c) + (x*s); //+-
    outMat9[8] = (z*z)*(1-c) + c;
}
