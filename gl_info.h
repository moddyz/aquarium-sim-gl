/*
 *  glinfo.h
 *  OpenGL State Information library
 *
 *  Created by Douglas Lanman and Matthew Hirsch.
 *  Presented in the "Build Your Own Glasses-free 3D Display" course at SIGGRAPH 2011.
 *  Copyright 2011.
 *
 */

#ifndef OPENGL_STATE_INFORMATION_LIBRARY_H
#define OPENGL_STATE_INFORMATION_LIBRARY_H

// --------------------------------------------------------------------------------
// Include statements.

#include <stdlib.h>    // standard I/O library
#include <stdio.h>     // standard library
#include "gl_include.h" // OpenGL include files

// --------------------------------------------------------------------------------
// Function prototypes.

// Convert OpenGL internal format enum to string.
const char* convertInternalFormatToString(GLenum format);

// Return renderbuffer parameters as string using glGetRenderbufferParameteriv.
const char* getRenderbufferParameters(GLuint id, char* buf, unsigned int buflen);

// Return texture parameters as string using glGetTexLevelParameteriv()
const char* getTextureParameters(GLuint id, char* buf, unsigned int buflen);

// Print the FBO details.
void printFramebufferInfo(const char* label);

// Check for FBO completeness.
GLboolean checkFramebufferStatus(void);

// Print OpenGL driver details.
GLboolean printDriverInfo(GLboolean);

// Print details for a subset of the OpenGL state.
// Note: This is useful for debugging while adapting existing code to work with a 3D display.
void printGLState(void);
void printglDepthFunc(void);
void printglAlphaTest(void);
void printglStencilTest(void);
void printglBlendParams(void);
void printglMatrixMode(void);
void printglCullFaceMode(void);

#endif
