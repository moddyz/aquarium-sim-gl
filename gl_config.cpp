#include "gl_config.h"

int setupGLConfig(GLConfig *config, const char* configFile){
	if(FILE* fp = fopen(configFile, "r")){
		char line[128], attribute[128];
		int flag, category = 0;
		while(fgets(line, 128, fp)){
			sscanf(line, "%s", attribute);
			if      (!strcmp(attribute, "[screen]"))     { category = 1; continue; }
			if      (!strcmp(attribute, "[camera]"))     { category = 2; continue; }
			else if (!strcmp(line, "\n"))      		     { category = 0; continue; }
			
			switch(category){
				case 1: // Screen Parameters
					if      (!strcmp(attribute, "width"))  sscanf(line, "%*s = %d", &config->screen.width);
					else if (!strcmp(attribute, "height")) sscanf(line, "%*s = %d", &config->screen.height);
					else if (!strcmp(attribute, "initX")) sscanf(line, "%*s = %d", &config->screen.initX);
					else if (!strcmp(attribute, "initY")) sscanf(line, "%*s = %d", &config->screen.initY);
					break;
				case 2: // Camera Parameters
					if      (!strcmp(attribute, "position_x"))  sscanf(line, "%*s = %f", &config->camera.position.x);
					else if (!strcmp(attribute, "position_y")) sscanf(line, "%*s = %f", &config->camera.position.y);
					else if (!strcmp(attribute, "position_z")) sscanf(line, "%*s = %f", &config->camera.position.z);
                    else if (!strcmp(attribute, "lookat_x"))  sscanf(line, "%*s = %f", &config->camera.lookAt.x);
					else if (!strcmp(attribute, "lookat_y")) sscanf(line, "%*s = %f", &config->camera.lookAt.y);
					else if (!strcmp(attribute, "lookat_z")) sscanf(line, "%*s = %f", &config->camera.lookAt.z);
                    else if (!strcmp(attribute, "up_x"))  sscanf(line, "%*s = %f", &config->camera.up.x);
					else if (!strcmp(attribute, "up_y")) sscanf(line, "%*s = %f", &config->camera.up.y);
					else if (!strcmp(attribute, "up_z")) sscanf(line, "%*s = %f", &config->camera.up.z);
                    else if (!strcmp(attribute, "frustum_left"))  sscanf(line, "%*s = %f", &config->camera.frustum.left);
					else if (!strcmp(attribute, "frustum_right")) sscanf(line, "%*s = %f", &config->camera.frustum.right);
					else if (!strcmp(attribute, "frustum_bottom")) sscanf(line, "%*s = %f", &config->camera.frustum.bottom);
					else if (!strcmp(attribute, "frustum_top")) sscanf(line, "%*s = %f", &config->camera.frustum.top);
					else if (!strcmp(attribute, "frustum_near")) sscanf(line, "%*s = %f", &config->camera.frustum.near);
					else if (!strcmp(attribute, "frustum_far")) sscanf(line, "%*s = %f", &config->camera.frustum.far);
					break;
			}
		}
		fclose(fp);
	}
	else{
		printf("Error: Unable to read \"%s\".\n", configFile);
		return -1;
	}
	return 0;
}

int configureCameraFrustum(GLConfig *config)
{
    float WHRatio = (float) config->screen.width / (float) config->screen.height;
    
    config->camera.frustum.left  = config->camera.frustum.bottom * WHRatio;
    config->camera.frustum.right = config->camera.frustum.top * WHRatio;
}

void printConfig(GLConfig *config)
{
    ScreenParameters* scr = &config->screen;
    CameraParameters* cam = &config->camera;
    ViewingFrustum*   fst = &config->camera.frustum;

    printf("\n");
    printf("Printing Configuration...\n");
    printf("Config -- Screen Dimensions (W,H): %d, %d\n", scr->width, scr->height);
    printf("Config -- Screen Initial Position (X,Y): %d, %d\n", scr->initX, scr->initY);
    
    printf("Config -- Camera Position (X,Y,Z): %f, %f, %f\n", cam->position.x, cam->position.y, cam->position.z);
    printf("Config -- Camera Look-at (X,Y,Z): %f, %f, %f\n", cam->lookAt.x, cam->lookAt.y, cam->lookAt.z);
    printf("Config -- Camera Up-vector (X,Y,Z): %f, %f, %f\n", cam->up.x, cam->up.y, cam->up.z);
    
    printf("Config -- Viewing Frustum Clipping Planes (L,R,B,T,N,F): %f, %f, %f, %f, %f, %f\n", fst->left, fst->right, fst->bottom, fst->top, fst->near, fst->far);
    printf("\n");
}
