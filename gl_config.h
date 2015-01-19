

// OpenGL Configuration Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector_types.h"

#ifndef _OPENGL_CONFIG_H
#define _OPENGL_CONFIG_H

typedef struct
{
    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;
} ViewingFrustum;

typedef struct
{
    int width;
    int height;
    int initX;
    int initY;
} ScreenParameters;

typedef struct
{
    v3f position;
    v3f lookAt;
    v3f up;
    ViewingFrustum frustum; 
} CameraParameters;

typedef struct
{
    ScreenParameters screen;
    CameraParameters camera;
} GLConfig;

int setupGLConfig(GLConfig* config, const char* configFile);
int configureCameraFrustum(GLConfig *config);
void printConfig(GLConfig *config);

#endif
