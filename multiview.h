#ifndef MULTIVIEW_EXTENSION
#define MULTIVIEW_EXTENSION

// Common Libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// GL Libs
#include "gl_include.h"
#include "gl_info.h"
#include "gl_util.h"
#include "gl_config.h"

#define MULTICAMERA_MODE_CONVERGE 0
#define MULTICAMERA_MODE_PARALLEL 1

typedef struct
{
    float       angle;
    int         numViews;
} AttenuatorParameters;

typedef struct 
{
    float       interaxialDistance; // separation distance between each view
    int         mode;
} MultiCameraParameters;

typedef struct
{
    AttenuatorParameters    attenuator;
    MultiCameraParameters   camera;
    GLuint*                 frameBuffer;
    GLuint                  viewTextures;
    GLuint                  depthStencilBuffer;
    ShaderProgram           shaderProgram;
} MultiviewSystem;

int configureMultiview(MultiviewSystem* multiview, const char* configFile);
int setupMultiview(MultiviewSystem *multiview, GLConfig* config, GLboolean frameBufferStatus);
int shutdownMultiview(MultiviewSystem *multiview);
int updateMultiview(MultiviewSystem* multiview, GLConfig* config);
int renderMultiview(MultiviewSystem* multiview, GLConfig* config, int view_index);
int displayMultiview(MultiviewSystem* multiview, GLConfig* config);



#endif
