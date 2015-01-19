#ifndef _OPENGL_UTILITY_H
#define _OPENGL_UTILITY_H

#include <stdlib.h>    // standard I/O library
#include <stdio.h>     // standard library
#include "gl_include.h"

// Shader Program Structure
typedef struct
{
    GLuint id;
    GLuint vert;
    GLuint geom; 
    GLuint frag;
} ShaderProgram;

// Read File
char* fileRead(const char* filename);

// Print information on Shader/Program object
void printShaderProgramLog(GLuint object);

// Build Shader Program from source files
int buildShaderProgram(ShaderProgram* program,
                       const char* vert_file,
                       const char* geom_file,
                       const char* frag_file);
#endif
