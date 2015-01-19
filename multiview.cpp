#include "multiview.h"

int configureMultiview(MultiviewSystem* multiview, const char* configFile){

	// Parse the configuration file.
	if(FILE* fp = fopen(configFile, "r")){
		char line[128], attribute[128];
		int flag, category = 0;
		while(fgets(line, 128, fp)){
			
			// Update current category.
			sscanf(line, "%s", attribute);
			if      (!strcmp(attribute, "[attenuator]"))     { category = 1; continue; }
			else if (!strcmp(attribute, "[camera]")) { category = 2; continue; }
			else if (!strcmp(line, "\n"))      		     { category = 0; continue; }
			
			// Set category attribute.
			switch(category){
				case 1:
					// Set the lenticular parameters.
					if (!strcmp(attribute, "angle"))	sscanf(line, "%*s = %f", &multiview->attenuator.angle);
					else if (!strcmp(attribute, "num_views"))	sscanf(line, "%*s = %d", &multiview->attenuator.numViews);
					break;
				case 2:
					// Set the camera parameters.
					if (!strcmp(attribute, "interaxial_distance"))	     sscanf(line, "%*s = %f", &multiview->camera.interaxialDistance);
					else if (!strcmp(attribute, "mode"))	sscanf(line, "%*s = %d", &multiview->camera.mode);
					break;
			}
			
		}
		
		// Close the configuration file.
		fclose(fp);
		
	}
	else{
		printf("Error: Unable to read \"%s\".\n", configFile);
		return -1;
	}
	
	// Return without errors.
	return 0;
}
int setupMultiview(MultiviewSystem *multiview, GLConfig* config, GLboolean frameBufferStatus)
{
    // Setup frame buffer for view storage
    glGenTextures(1, &multiview->viewTextures);
    glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, multiview->viewTextures);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, GL_RGBA8, config->screen.width,
                 config->screen.height, multiview->attenuator.numViews, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, 0);
	
    // Initialize a depth/stencil renderbuffer object and attach to the FBO.
	glGenRenderbuffers(1, &multiview->depthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, multiview->depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, config->screen.width, config->screen.height);

    multiview->frameBuffer = (GLuint*) malloc(multiview->attenuator.numViews*sizeof(GLuint));
    glGenFramebuffers(multiview->attenuator.numViews, multiview->frameBuffer);
    for (int i = 0; i < multiview->attenuator.numViews; ++i)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, multiview->frameBuffer[i]);
        glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, multiview->viewTextures, 0, i);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multiview->depthStencilBuffer);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multiview->depthStencilBuffer);
        if (frameBufferStatus)
        {
            char label[15];
            snprintf(label, 15, "View %d", i);
            printFramebufferInfo(label);
        }
        if (checkFramebufferStatus() == GL_FALSE)
        {
            printf("Error: FBO status is not complete.\n");
            return -1;
        }
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    // Set-up shader program
    GLuint *program = &multiview->shaderProgram.id;
    if (buildShaderProgram(&multiview->shaderProgram, 
                                "shaders/multiplexer.v.glsl",
                                NULL,
                                "shaders/multiplexer.f.glsl") == -1) return -1;
}

int shutdownMultiview(MultiviewSystem *multiview)
{
    // Free textures
    glDeleteTextures(1, &multiview->viewTextures);
    // Free frame buffer
    glDeleteFramebuffers(multiview->attenuator.numViews, multiview->frameBuffer);
	// Free depth/stencil renderbuffer object.
	glDeleteRenderbuffers(1, &multiview->depthStencilBuffer);
    free(multiview->frameBuffer);
    
    // Free shader program
    glDetachShader(multiview->shaderProgram.id, multiview->shaderProgram.vert);
    glDetachShader(multiview->shaderProgram.id, multiview->shaderProgram.frag);
    glDeleteShader(multiview->shaderProgram.vert);
    glDeleteShader(multiview->shaderProgram.frag);
    glDeleteProgram(multiview->shaderProgram.id);
    
    // Return without errors
    return 0;
}

int renderMultiview(MultiviewSystem* multiview, GLConfig* config, int view_index)
{
    float separation = multiview->camera.interaxialDistance / multiview->attenuator.numViews;
	float            x = config->camera.position.x+view_index*separation-
	                     ((multiview->attenuator.numViews-1)*separation/2);
	glViewport(0, 0, config->screen.width, config->screen.height);
	
    ViewingFrustum *fst = &config->camera.frustum;
    CameraParameters *cam = &config->camera;
    
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(fst->left, fst->right, fst->bottom, fst->top, fst->near, fst->far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    float lookAtX = cam->lookAt.x;
    if (multiview->camera.mode == 1)
        lookAtX = x;

	gluLookAt(x,             cam->position.y, cam->position.z,
              lookAtX, cam->lookAt.y,   cam->lookAt.z,
              cam->up.x,     cam->up.y,       cam->up.z);
}

int displayMultiview(MultiviewSystem *multiview, GLConfig* config)
{
    // Save the OpenGL state (for modes altered by the lenticular display function).
	GLint     matrix_mode;
	GLint     cull_face_mode;
	GLboolean depth_test;
	GLboolean lighting;
	glGetIntegerv(GL_MATRIX_MODE,    &matrix_mode);
	glGetIntegerv(GL_CULL_FACE_MODE, &cull_face_mode);
	depth_test = glIsEnabled(GL_DEPTH_TEST);
	lighting   = glIsEnabled(GL_LIGHTING);

	// Disable FBO rendering mode.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	// Disable depth testing and lighting.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glCullFace(GL_BACK);
	// Clear the color buffer.
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Enable the lenticular screen multiview shader program.
	glUseProgram(multiview->shaderProgram.id);
	
	// Bind 2D texture array storing rendered views and view masks to lenticular multiview program.
	glUniform1i(glGetUniformLocation(multiview->shaderProgram.id, "viewTextures"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, multiview->viewTextures);
	
	// Set additional uniform variables.
	glUniform1f(glGetUniformLocation(multiview->shaderProgram.id, "angle"),    multiview->attenuator.angle);
	glUniform1i(glGetUniformLocation(multiview->shaderProgram.id, "numViews"), multiview->attenuator.numViews);
	
	// Display the interlaced image.
	glViewport(0, 0, config->screen.width, config->screen.height);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, config->screen.width, 0, config->screen.height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);	glVertex3f(                 0.0,                   0.0, 0.0);
		glTexCoord2f(1.0, 0.0);	glVertex3f(config->screen.width,                   0.0, 0.0);
		glTexCoord2f(1.0, 1.0);	glVertex3f(config->screen.width, config->screen.height, 0.0);
		glTexCoord2f(0.0, 1.0);	glVertex3f(                 0.0, config->screen.height, 0.0);
	glEnd();
	
	// Disable the shader program.
	glUseProgram(0);
	
	// Unbind 2D texture array.
	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, 0);	
	
	// Restore the OpenGL state.
	glPopMatrix();
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glCullFace(cull_face_mode);
	glMatrixMode(matrix_mode);
	if(depth_test)
        glEnable(GL_DEPTH_TEST);
	if(lighting)
        glEnable(GL_LIGHTING);

	glActiveTexture(GL_TEXTURE0);

}
