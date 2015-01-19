/*
 *  glinfo.cpp
 *  OpenGL State Information library
 *
 *  Created by Douglas Lanman and Matthew Hirsch.
 *  Presented in the "Build Your Own Glasses-free 3D Display" course at SIGGRAPH 2011.
 *  Copyright 2011.
 *
 *  Portions of this file were adapted from Song Ho Ahn's (song.ahn@gmail.com) 
 *  fboStencil example.
 *
 */

// Include OpenGL State Information library header.
#include "gl_info.h"

// Convert OpenGL internal format enum to string.
const char* convertInternalFormatToString(GLenum format){

    #define INTLFMT_CASE(id) case id: return #id
    switch(format){
            INTLFMT_CASE(GL_STENCIL_INDEX);
            INTLFMT_CASE(GL_STENCIL_INDEX1);
            INTLFMT_CASE(GL_STENCIL_INDEX4);
            INTLFMT_CASE(GL_STENCIL_INDEX8);
            INTLFMT_CASE(GL_STENCIL_INDEX16);
            INTLFMT_CASE(GL_DEPTH24_STENCIL8);
            INTLFMT_CASE(GL_DEPTH_COMPONENT);
            INTLFMT_CASE(GL_DEPTH_COMPONENT16);
            INTLFMT_CASE(GL_DEPTH_COMPONENT24);
            INTLFMT_CASE(GL_DEPTH_COMPONENT32);
            INTLFMT_CASE(GL_ALPHA);
            INTLFMT_CASE(GL_RGB);
            INTLFMT_CASE(GL_RGBA);
            INTLFMT_CASE(GL_LUMINANCE);
            INTLFMT_CASE(GL_LUMINANCE_ALPHA);
            INTLFMT_CASE(GL_ALPHA4);
            INTLFMT_CASE(GL_ALPHA8);
            INTLFMT_CASE(GL_ALPHA12);
            INTLFMT_CASE(GL_ALPHA16);
            INTLFMT_CASE(GL_LUMINANCE4);
            INTLFMT_CASE(GL_LUMINANCE8);
            INTLFMT_CASE(GL_LUMINANCE12);
            INTLFMT_CASE(GL_LUMINANCE16);
            INTLFMT_CASE(GL_LUMINANCE4_ALPHA4);
            INTLFMT_CASE(GL_LUMINANCE6_ALPHA2);
            INTLFMT_CASE(GL_LUMINANCE8_ALPHA8);
            INTLFMT_CASE(GL_LUMINANCE12_ALPHA4);
            INTLFMT_CASE(GL_LUMINANCE12_ALPHA12);
            INTLFMT_CASE(GL_LUMINANCE16_ALPHA16);
            INTLFMT_CASE(GL_INTENSITY);
            INTLFMT_CASE(GL_INTENSITY4);
            INTLFMT_CASE(GL_INTENSITY8);
            INTLFMT_CASE(GL_INTENSITY12);
            INTLFMT_CASE(GL_INTENSITY16);
            INTLFMT_CASE(GL_R3_G3_B2);
            INTLFMT_CASE(GL_RGB4);
            INTLFMT_CASE(GL_RGB5);
            INTLFMT_CASE(GL_RGB8);
            INTLFMT_CASE(GL_RGB10);
            INTLFMT_CASE(GL_RGB12);
            INTLFMT_CASE(GL_RGB16);
            INTLFMT_CASE(GL_RGBA2);
            INTLFMT_CASE(GL_RGBA4);
            INTLFMT_CASE(GL_RGB5_A1);
            INTLFMT_CASE(GL_RGBA8);
            INTLFMT_CASE(GL_RGB10_A2);
            INTLFMT_CASE(GL_RGBA12);
            INTLFMT_CASE(GL_RGBA16);
        default:
            return "Unknown format.";
    }
    return "Uknown format.";
}
#undef INTLFMT_CASE

// Return renderbuffer parameters as string using glGetRenderbufferParameteriv.
const char* getRenderbufferParameters(GLuint id, char* buf, unsigned int buflen){
  
    int width, height, format;
    if(glIsRenderbufferEXT(id) == GL_FALSE)
        return "Not a renderbuffer object.";
    glBindRenderbufferEXT(GL_RENDERBUFFER, id);
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    glGetRenderbufferParameterivEXT(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &format); 
    glBindRenderbufferEXT(GL_RENDERBUFFER, 0);
    snprintf(buf, buflen, "%dx%d, %s", width, height, convertInternalFormatToString(format));
    return buf;
    
}

// Return texture parameters as string using glGetTexLevelParameteriv()
const char* getTextureParameters(GLuint id, char* buf, unsigned int buflen){

    int width, height, format;
    if(glIsTexture(id) == GL_FALSE)
        return "Not texture object";
    glBindTexture(GL_TEXTURE_2D, id);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    glBindTexture(GL_TEXTURE_2D, 0);
    snprintf(buf, buflen, "%dx%d, %s", width, height, convertInternalFormatToString(format));
    return buf;

}

// Print the FBO details.
void printFramebufferInfo(const char* label){
    
    // Allocate temporary variables.
    int colorBufferCount = 0;
    int objectType;
    int objectId;
    int i;
    const unsigned int buflen = 200;
    char buf[buflen];

    // Display FBO details header.
    if(label == NULL)
        printf("\n***** FBO STATUS *****\n");
    else
        printf("\n***** %s FBO STATUS  *****\n", label);

    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &colorBufferCount);
    printf("Max Number of Color Buffer Attachment Points: %d\n",colorBufferCount);

    // Print details regarding the colorbuffer attachable image.
    for(i=0; i<colorBufferCount; ++i){
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
		 			                          GL_COLOR_ATTACHMENT0+i,
					                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
					                          &objectType);
        if(objectType != GL_NONE) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                  GL_COLOR_ATTACHMENT0+i,
					                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
					                              &objectId);
            printf("Color Attachment %d: ",i);
            if(objectType == GL_TEXTURE)
                printf("GL_TEXTURE, %s\n", getTextureParameters(objectId,buf,buflen));
            else if(objectType == GL_RENDERBUFFER)
                printf("GL_RENDERBUFFER, %s\n",getRenderbufferParameters(objectId,buf,buflen));
        }
    }

    // Print details regarding the depthbuffer attachable image.
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER,
	  				                         GL_DEPTH_ATTACHMENT,
					                         GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
					                         &objectType);
    if(objectType != GL_NONE) {
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER,
					                             GL_DEPTH_ATTACHMENT,
					                             GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
					                             &objectId);
        printf("Depth Attachment: ");
        switch(objectType){
            case GL_TEXTURE:
                printf("GL_TEXTURE, %s\n", getTextureParameters(objectId, buf, buflen));
                break;
            case GL_RENDERBUFFER:
                printf("GL_RENDERBUFFER, %s\n", getRenderbufferParameters(objectId, buf, buflen));
                break;
        }
    }

    // Print details regarding the stencilbuffer attachable image.
    glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER,
					                         GL_STENCIL_ATTACHMENT,
					                         GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
					                         &objectType);
    if(objectType != GL_NONE){
        glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER,
		 			                             GL_STENCIL_ATTACHMENT,
                                                GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
					                            &objectId);
        printf("Stencil Attachment: ");
        switch(objectType){
            case GL_TEXTURE:
                printf("GL_TEXTURE, %s\n", getTextureParameters(objectId, buf, buflen));
                break;
            case GL_RENDERBUFFER:
                printf("GL_RENDERBUFFER, %s\n", getRenderbufferParameters(objectId, buf, buflen));
                break;
        }
    }
    printf("\n");
    
}

// Check for FBO completeness.
GLboolean checkFramebufferStatus(void){
    
    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    switch(status){
        case GL_FRAMEBUFFER_COMPLETE:
            return GL_TRUE;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            printf("[ERROR] Framebuffer incomplete: Attachment is not complete.\n");
            return GL_FALSE;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf("[ERROR] Framebuffer incomplete: No image is attached to FBO.\n");
            return GL_FALSE;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("[ERROR] Framebuffer incomplete: Attached images have different dimensions.\n");
            return GL_FALSE;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("[ERROR] Framebuffer incomplete: Color attached images have different internal formats.\n");
            return GL_FALSE;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            printf("[ERROR] Framebuffer incomplete: Draw buffer.\n");
            return GL_FALSE;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            printf("[ERROR] Framebuffer incomplete: Read buffer.\n");
            return GL_FALSE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            printf("[ERROR] Framebuffer incomplete. Unsupported by FBO implementation.\n");
            return GL_FALSE;
        default:
            printf("[ERROR] Framebuffer incomplete. Unknown error.\n");
            return GL_FALSE;
    }
    
}

// Print OpenGL driver details.
GLboolean printDriverInfo(GLboolean print_extensions){

    #define CHECK_ERROR(str) if (str==NULL) {printf("Error getting driver information\n"); return GL_FALSE;} 
    
    // Allocate temporary variables.
    char *str = NULL;
    int res = 0;

    // Display OpenGL driver details header.
    printf("\n");
    printf("OpenGL Driver Information\n");
    printf("=========================\n");
    str = (char*)glGetString(GL_VENDOR);
    CHECK_ERROR(str);
    printf("Vendor: %s\n",str);
    str = (char*)glGetString(GL_VERSION);
    CHECK_ERROR(str);
    printf("Version: %s\n",str);
    str = (char*)glGetString(GL_RENDERER);
    CHECK_ERROR(str);
    printf("Renderer: %s\n",str);
    if(print_extensions) {
        str = (char*)glGetString(GL_EXTENSIONS);
        CHECK_ERROR(str);
        printf("Extensions:\n----------------------\n%s\n----------------------\n",str);
    }
  
    // Get number of color bits.
    printf("\nColor Buffer Bits:\n");
    glGetIntegerv(GL_RED_BITS, &res);
    printf("\tRed  : %d\n",res);
    glGetIntegerv(GL_GREEN_BITS, &res);
    printf("\tBlue : %d\n",res);
    glGetIntegerv(GL_BLUE_BITS, &res);
    printf("\tGreen: %d\n",res);
    glGetIntegerv(GL_ALPHA_BITS, &res);
    printf("\tAlpha: %d\n",res);

    // Get number of depth bits.
    glGetIntegerv(GL_DEPTH_BITS, &res);
    printf("\nDepth Bits: %d\n",res);

    // Get number of stecil bits.
    glGetIntegerv(GL_STENCIL_BITS, &res);
    printf("Stencil Bits: %d\n",res);

    // Get maximum number of lights allowed.
    glGetIntegerv(GL_MAX_LIGHTS, &res);
    printf("Max Lights: %d\n",res);

    // Get maximum texture resolution.
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &res);
    printf("Max Texture: %dx%d\n",res,res);

    // Get maximum number of clipping planes.
    glGetIntegerv(GL_MAX_CLIP_PLANES, &res);
    printf("Max Clip Planes: %d\n",res);

    // Get maximum modelview and projection matrix stacks.
    glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &res);
    printf("\nModelview Stack Depth: %d\n",res);
    glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &res);
    printf("Projection Stack Depth: %d\n",res);
    glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH, &res);
    printf("Attrib Stack Depth: %d\n",res);

    // get maximum depth of texture stacks.
    glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &res);
    printf("Texture Stack Depth: %d\n",res);
    printf("\n");
    
    // Return without errors.
    return GL_TRUE;
  
}

// Define PGL case function details.
#define PGLCASE(id) case id: p = #id; break

// Print OpenGL depth function details.
void printglDepthFunc(void){
    
    GLint test;
    GLboolean m;
    const char *p;
    glGetIntegerv(GL_DEPTH_FUNC,&test);
    switch(test){
            PGLCASE(GL_NEVER);
            PGLCASE(GL_LESS);
            PGLCASE(GL_EQUAL);
            PGLCASE(GL_LEQUAL);
            PGLCASE(GL_GREATER);
            PGLCASE(GL_NOTEQUAL);
            PGLCASE(GL_GEQUAL);
            PGLCASE(GL_ALWAYS);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_DEPTH_FUNC: %s\n", p);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &m);
    printf("\t\tGL_DEPTH_WRITEMASK: %s\n", m ? "GL_TRUE" : "GL_FALSE");
    
}

// Print OpenGL alpha test details.
void printglAlphaTest(void){
    
    GLint test;
    const char *p;
    glGetIntegerv(GL_ALPHA_TEST_FUNC,&test);
    switch(test){
            PGLCASE(GL_NEVER);
            PGLCASE(GL_LESS);
            PGLCASE(GL_EQUAL);
            PGLCASE(GL_LEQUAL);
            PGLCASE(GL_GREATER);
            PGLCASE(GL_NOTEQUAL);
            PGLCASE(GL_GEQUAL);
            PGLCASE(GL_ALWAYS);
    default:
            p = "Unknown";
    }
    printf("\t\tGL_ALPHA_TEST_FUNC: %s\n", p);
    
}

// Print OpenGL stencil test details.
void printglStencilTest(void){
    
    GLint test;
    const char *p;
    glGetIntegerv(GL_STENCIL_FUNC,&test);
    switch(test){
            PGLCASE(GL_NEVER);
            PGLCASE(GL_LESS);
            PGLCASE(GL_EQUAL);
            PGLCASE(GL_LEQUAL);
            PGLCASE(GL_GREATER);
            PGLCASE(GL_NOTEQUAL);
            PGLCASE(GL_GEQUAL);
            PGLCASE(GL_ALWAYS);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_STENCIL_FUNC: %s\n", p);
    
}

// Print OpenGL blending  details.
void printglBlendParams(void){
    
    GLint test[4];
    const char *p;
    glGetIntegerv(GL_BLEND_COLOR,test);
    printf("\t\tglBlendColor: R:%d G:%d B:%d A:%d\n",test[0],test[1],test[2],test[3]);
    glGetIntegerv(GL_BLEND_DST,test);
    switch(test[0]){
            PGLCASE(GL_ZERO);
            PGLCASE(GL_ONE);
            PGLCASE(GL_SRC_COLOR);
            PGLCASE(GL_ONE_MINUS_SRC_COLOR);
            PGLCASE(GL_SRC_ALPHA);
            PGLCASE(GL_ONE_MINUS_SRC_ALPHA);
            PGLCASE(GL_DST_ALPHA);
            PGLCASE(GL_ONE_MINUS_DST_ALPHA);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_BLEND_DST: %s\n",p);
    glGetIntegerv(GL_BLEND_SRC,test);
    switch(test[0]){
            PGLCASE(GL_ZERO);
            PGLCASE(GL_ONE);
            PGLCASE(GL_SRC_COLOR);
            PGLCASE(GL_ONE_MINUS_SRC_COLOR);
            PGLCASE(GL_SRC_ALPHA);
            PGLCASE(GL_ONE_MINUS_SRC_ALPHA);
            PGLCASE(GL_DST_ALPHA);
            PGLCASE(GL_ONE_MINUS_DST_ALPHA);
            PGLCASE(GL_SRC_ALPHA_SATURATE);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_BLEND_SRC: %s\n",p);
    glGetIntegerv(GL_BLEND_EQUATION, test);
    switch(test[0]){
            PGLCASE(GL_FUNC_ADD);
            PGLCASE(GL_MIN);
            PGLCASE(GL_MAX);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_BLEND_EQUATION: %s\n", p);
    
}

// Print OpenGL matrix mode details.
void printglMatrixMode(void){
 
    GLint test;
    const char *p;
    glGetIntegerv(GL_MATRIX_MODE,&test);
    switch(test){
            PGLCASE(GL_MODELVIEW);
            PGLCASE(GL_PROJECTION);
            PGLCASE(GL_TEXTURE);
            PGLCASE(GL_COLOR);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_MATRIX_MODE: %s\n", p);
}

// Print OpenGL culling mode details.
void printglCullFaceMode(void){
    
    GLint test;
    const char *p;
    glGetIntegerv(GL_CULL_FACE_MODE,&test);
    switch(test){
            PGLCASE(GL_FRONT);
            PGLCASE(GL_BACK);
            PGLCASE(GL_FRONT_AND_BACK);
        default:
            p = "Unknown";
    }
    printf("\t\tGL_CULL_FACE_MODE: %s\n", p);
    
}

// Print details for a subset of the OpenGL state.
// Note: This is useful for debugging while adapting existing code to work with a 3D display.
void printGLState(void){
    
    #define GLCHECK(id) printf("\t" #id ": %s\n",glIsEnabled(id)?"TRUE":"FALSE")
    int res;
    printf("========== OpenGL State ==========\n");
    printglMatrixMode();
    GLCHECK(GL_ALPHA_TEST);
    printglAlphaTest();
    GLCHECK(GL_AUTO_NORMAL);
    GLCHECK(GL_BLEND);
    printglBlendParams();
    glGetIntegerv(GL_MAX_CLIP_PLANES, &res);
    if(res>0) GLCHECK(GL_CLIP_PLANE0);
    if(res>1) GLCHECK(GL_CLIP_PLANE1);
    if(res>2) GLCHECK(GL_CLIP_PLANE2);
    if(res>3) GLCHECK(GL_CLIP_PLANE3);
    if(res>4) GLCHECK(GL_CLIP_PLANE4);
    if(res>5) GLCHECK(GL_CLIP_PLANE5);
    GLCHECK(GL_COLOR_ARRAY);
    GLCHECK(GL_COLOR_LOGIC_OP);
    GLCHECK(GL_COLOR_MATERIAL);
    GLCHECK(GL_COLOR_TABLE);
    GLCHECK(GL_CONVOLUTION_1D);
    GLCHECK(GL_CONVOLUTION_2D);
    GLCHECK(GL_CULL_FACE);
    printglCullFaceMode();
    GLCHECK(GL_DEPTH_TEST);
    printglDepthFunc();
    GLCHECK(GL_DITHER);
    GLCHECK(GL_EDGE_FLAG_ARRAY);
    GLCHECK(GL_FOG);
    GLCHECK(GL_HISTOGRAM);
    glGetIntegerv(GL_MAX_LIGHTS,&res);
    if(res>0) GLCHECK(GL_LIGHT0);
    if(res>1) GLCHECK(GL_LIGHT1);
    if(res>2) GLCHECK(GL_LIGHT2);
    if(res>3) GLCHECK(GL_LIGHT3);
    if(res>4) GLCHECK(GL_LIGHT4);
    if(res>5) GLCHECK(GL_LIGHT5);
    if(res>6) GLCHECK(GL_LIGHT6);
    if(res>7) GLCHECK(GL_LIGHT7);
    GLCHECK(GL_LIGHTING);
    GLCHECK(GL_LINE_SMOOTH);
    GLCHECK(GL_LINE_STIPPLE);
    GLCHECK(GL_MINMAX);
    GLCHECK(GL_NORMAL_ARRAY);
    GLCHECK(GL_NORMALIZE);
    GLCHECK(GL_POINT_SMOOTH);
    GLCHECK(GL_POLYGON_SMOOTH);
    GLCHECK(GL_POLYGON_OFFSET_FILL);
    GLCHECK(GL_POLYGON_OFFSET_LINE);
    GLCHECK(GL_POLYGON_OFFSET_POINT);
    GLCHECK(GL_POLYGON_STIPPLE);
    GLCHECK(GL_POST_COLOR_MATRIX_COLOR_TABLE);
    GLCHECK(GL_POST_CONVOLUTION_COLOR_TABLE);
    GLCHECK(GL_RESCALE_NORMAL);
    GLCHECK(GL_SEPARABLE_2D);
    GLCHECK(GL_SCISSOR_TEST);
    GLCHECK(GL_STENCIL_TEST);
    printglStencilTest();
    GLCHECK(GL_TEXTURE_1D);
    GLCHECK(GL_TEXTURE_2D);
    GLCHECK(GL_TEXTURE_3D);
    GLCHECK(GL_TEXTURE_COORD_ARRAY);
    GLCHECK(GL_TEXTURE_GEN_Q);
    GLCHECK(GL_TEXTURE_GEN_R);
    GLCHECK(GL_TEXTURE_GEN_S);
    GLCHECK(GL_TEXTURE_GEN_T);
    GLCHECK(GL_VERTEX_ARRAY);
    printf("========== OpenGL State End ======\n");

}
