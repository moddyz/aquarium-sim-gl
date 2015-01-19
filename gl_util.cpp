// OpenGL Utilities Implementation

#include "gl_util.h"

// Read and return content
char* fileRead(const char* filename)
{
    FILE* input = fopen(filename, "rb");
    if (input == NULL) return NULL;

    if (fseek(input, 0, SEEK_END) == -1) return NULL;
    long size = ftell(input);
    if (size == -1) return NULL;
    if (fseek(input, 0, SEEK_SET) == -1) return NULL;

    char *content = (char*) malloc( (size_t) size + 1);
    if (content == NULL) return NULL;

    fread(content, 1, (size_t)size, input);
    if (ferror(input))
    {
        free(content);
        return NULL;
    }

    fclose(input);

    content[size] = '\0';
    return content;
}

// Print information about OpenGL object
void printShaderProgramLog(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else 
    {
        fprintf(stderr, "printlog: Not a shader or a program\n");
        return;
    }
    
    char* log = (char*) malloc(log_length);
    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    fprintf(stderr, "%s", log);
    free(log);
}

int buildShaderProgram(ShaderProgram* program,
                       const char* vert_file,
                       const char* geom_file,
                       const char* frag_file)
{
    int shader_count; 
    program->id = glCreateProgram();
   
    if (vert_file != NULL)
    {
        char *vs = fileRead(vert_file);
        const char* v = vs;
        program->vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(program->vert, 1, &v, NULL);
        free(vs);
        
        glCompileShader(program->vert);
        printShaderProgramLog(program->vert);
        
        glAttachShader(program->id, program->vert);
        
        printf("Shader: Vertex shader %s successfully compiled & attached to shader program ID %d.\n", vert_file, program->id);
        
        ++shader_count;
    }
    else
    {
        printf("Shader: Missing vertex shader in program ID %d.\n", program->id);
    }

    if (geom_file != NULL)
    {
        char *gs = fileRead(geom_file);

        const char* g = gs;
        program->geom = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(program->geom, 1, &g, NULL);
        free(gs);

        glCompileShader(program->geom);
        printShaderProgramLog(program->geom);
        
        glAttachShader(program->id, program->geom);
        
        printf("Shader: Geometry shader %s successfully compiled & attached to shader program ID %d.\n", geom_file, program->id);
       
        ++shader_count;
    }
    else
    {
        printf("Shader: Missing geometry shader in program ID %d.\n", program->id);
    }

    if (frag_file != NULL)
    {
        char *fs = fileRead(frag_file);

        const char* f = fs;
        program->frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(program->frag, 1, &f, NULL);
        free(fs);

        glCompileShader(program->frag);
        printShaderProgramLog(program->frag);
        
        glAttachShader(program->id, program->frag);
        
        printf("Shader: Fragment shader %s successfully compiled & attached to shader program ID %d.\n", frag_file, program->id);
       
        ++shader_count;
    }
    else
    {
        printf("Shader: Missing fragment shader in program ID %d.\n", program->id);
    }

    if (shader_count > 0)
    { 
        // Read shader sources
        glLinkProgram(program->id);
        printShaderProgramLog(program->id);
        
        // Check validity
        GLint valid;
        glValidateProgram(program->id);
        glGetProgramiv(program->id, GL_VALIDATE_STATUS, &valid);
        
        if (valid != GL_TRUE)
        {
            printf("Error: Shader program is not valid.\n");
            return -1;
        }
    }
    else
    {
        glDeleteProgram(program->id);
        printf("ShaderError: Shader sources not found. Destroying Shader Program.\n");
        return -1;
    }
    
    // Shader program build success
    return 0;
}
