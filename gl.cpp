// Common Libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <list>
#include "time.h"

// GL Libs
#include "simulation.h"
#include "gl_include.h"
#include "gl_config.h"
#include "gl_info.h"
#include "gl_util.h"
#include "multiview.h"
#include "glm.h"
#include "vector_types.h"

#define PI 3.14159265359

GLMmodel* ground;
GLMmodel* environment;
GLMmodel* fish;

GLConfig config;
MultiviewSystem multiview;
SimulationSystem sim;
float last_time;

typedef struct
{
    GLfloat translate[3];
    GLfloat rotate[3];
    GLfloat scale[3];
} Transform;


int lights()
{
    // Basic Lighting System

    GLfloat light0_position[] = {0.0, 100.0, 0.0, 1.0};
    GLfloat light0_ambient[]  = {0.05, 0.4, 0.9, 1.0};
    GLfloat light0_diffuse[]  = {0.0, 0.2, 1.8, 1.0};
    GLfloat light0_specular[] = {0.05, 0.05, 0.2, 1.0};
   
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    GLfloat light1_position[] = {0.0, 100.0, 80.0, 1.0};
    GLfloat light1_ambient[]  = {0.0, 0.0, 0.0, 1.0};
    GLfloat light1_diffuse[]  = {0.0, 0.5, 0.8, 1.0};
    GLfloat light1_specular[] = {0.05, 0.05, 0.2, 1.0};
   
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    // Return 0 on Success
    return 0;
}

int shaders()
{
    // Basic Shader Setup


    // Return 0 on Success
    return 0;
}

int models()
{ 
    // Basic Model Setup
    ground = glmReadOBJ("models/groundc.obj");
    environment = glmReadOBJ("models/environment.obj");
    fish = glmReadOBJ("models/fish01.obj");

    glmUnitize(fish);
    glmScale(fish, 3.0);

    glmFacetNormals(fish);
    glmVertexNormals(fish, 90);
    // Return 0 on Success
    return 0;
}

int initialize()
{
    // Fix viewing frustum
    configureCameraFrustum(&config);
    printConfig(&config);

    // Multiview Set-up
    setupMultiview(&multiview, &config, 0);

    // Simulation Set-up
    initializeSimulation(&sim);

    // GL Initialization Calls
	glEnable(GL_DEPTH_TEST); // Enable depth testing.
    glEnable(GL_LIGHTING);   // Enable lighting
    glEnable(GL_LIGHT0);     // Enable LIGHT0
    glEnable(GL_LIGHT1);     // Enable LIGHT0
    glEnable(GL_NORMALIZE);  // Prevent scaling from changing normals.
    //glEnable(GL_CULL_FACE);  // Enable culling.
	//glCullFace(GL_BACK);     // Cull back-facing facets.
	glClearColor(0.04f, 0.25f, 0.55f, 1.0f); //Set the clearing color.
    
    models();   // Setup Models
    lights();   // Setup Lighting system
    shaders();  // Setup Shaders

    // TODO Application Initialization Calls
	last_time = 0; // Simple counter for animation
    return 0;
}

void shutdown(void)
{
    // Shutdown
}

void reshape(int width, int height)
{
    ViewingFrustum *fst = &config.camera.frustum;
    CameraParameters *cam = &config.camera;
    
    config.screen.width = width;
    config.screen.height = height;
    
    glViewport(0, 0, config.screen.width, config.screen.height);
}

void idle()
{
    float time = glutGet(GLUT_ELAPSED_TIME);
    float delta = (time - last_time) / 1000.0;

    stepSimulation(&sim, delta);

    last_time = time;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    float interaxialMod = (multiview.camera.mode == 0) ? 0.1 : 0.01;

    switch (key)
    {
        case '1':
            multiview.camera.mode = 0;
            printf("Multicam Mode --- Converging\n");
            break;
        case '2':
            multiview.camera.mode = 1;
            printf("Multicam Mode --- Parallel\n");
        case ']':
            multiview.camera.interaxialDistance += interaxialMod;
            printf("Multicam Interaxial Distance --- %f \n", multiview.camera.interaxialDistance);
            break;
        case '[':
            multiview.camera.interaxialDistance -= interaxialMod;
            printf("Multicam Interaxial Distance --- %f \n", multiview.camera.interaxialDistance);
            break;
        case '>':
            config.camera.lookAt.z += interaxialMod * 10;
            printf("Multicam Focal Length --- %f \n", fabs(config.camera.position.z - config.camera.lookAt.z) );
            break;
        case '<':
            config.camera.lookAt.z -= interaxialMod * 10;
            printf("Multicam Focal Length --- %f \n", fabs(config.camera.position.z - config.camera.lookAt.z) );
            break;
    }

    glutPostRedisplay();
}

void specialKey(int key, int x, int y)
{
    float moveStep = 0.15;
    switch(key)
    {
        case GLUT_KEY_LEFT:
            config.camera.position.x -= moveStep;
            config.camera.lookAt.x -= moveStep;
            printf("Base Camera Position (X,Y,Z) --- %f, %f, %f\n", config.camera.position.x, config.camera.position.y, config.camera.position.z);
            break;
        case GLUT_KEY_RIGHT:
            config.camera.position.x += moveStep;
            config.camera.lookAt.x += moveStep;
            printf("Base Camera Position (X,Y,Z) --- %f, %f, %f\n", config.camera.position.x, config.camera.position.y, config.camera.position.z);
            break;
        case GLUT_KEY_UP:
            config.camera.position.z -= moveStep;
            config.camera.lookAt.z -= moveStep;
            printf("Base Camera Position (X,Y,Z) --- %f, %f, %f\n", config.camera.position.x, config.camera.position.y, config.camera.position.z);
            break;
        case GLUT_KEY_DOWN:
            config.camera.position.z += moveStep;
            config.camera.lookAt.z += moveStep;
            printf("Base Camera Position (X,Y,Z) --- %f, %f, %f\n", config.camera.position.x, config.camera.position.y, config.camera.position.z);
            break;
    }
}

void camera()
{
    ViewingFrustum* fst = &config.camera.frustum;
    CameraParameters* cam = &config.camera;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(fst->left, fst->right, fst->bottom, fst->top, fst->near, fst->far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam->position.x, cam->position.y, cam->position.z,
              cam->lookAt.x, cam->lookAt.y, cam->lookAt.z,
              cam->up.x, cam->up.y, cam->up.z);
}


void display_model(void)
{
    // Draw Environment 
    glmDraw(environment, GLM_SMOOTH);
    glmDraw(ground, GLM_SMOOTH);
    
    std::list<Body>::iterator body = sim.bodyContainer.begin();
    // Draw each body as fish
    for ( ; body != sim.bodyContainer.end(); body++)
    {
        glColor3f (1.0, 0.0, 0.0);
        v3f v;
        vectorCpyv(&v, &((*body).velocity));
        vectorNormalize(&v);
        float zx = sqrt(v.x * v.x + v.z * v.z);
        float theta = atan2(zx, v.y) * 180.0 / PI;
        float phi = atan2(v.x, v.z) * 180.0 / PI;
        
        // get velocity vector z 
        glPushMatrix();
        glTranslatef(body->position.x, body->position.y, body->position.z);
        //glRotatef(90.0 - theta, 1.0, 0.0, 0.0);
        glRotatef(phi, 0.0, 1.0, 0.0);
        
        glmDraw(fish, GL_COLOR);
        //glutSolidCube(3.0);
        glPopMatrix();
    }
}

void display(void)
{
    for (int i = 0; i < multiview.attenuator.numViews; ++i)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, multiview.frameBuffer[i]);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderMultiview(&multiview, &config, i);

        display_model();        
    }

    displayMultiview(&multiview, &config);

    glutSwapBuffers();
}


int main(int argc, char* argv[])
{
    // Set Configurations
    setupGLConfig(&config, "gl.cfg");
    configureMultiview(&multiview, "multiview.cfg");
    configureSimulation(&sim, "simulation.cfg");

    // GLUT Init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowPosition(config.screen.initX, config.screen.initY);
    glutInitWindowSize(config.screen.width, config.screen.height);
    glutCreateWindow("OpenGL Application");

    // GLEW Init
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return EXIT_FAILURE;
    }
    
    if (initialize() == 0)
    {
        // Callbacks
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutIdleFunc(idle);
        
        // Input
        glutKeyboardFunc(keyboard);
        glutSpecialFunc(specialKey);

        glutMainLoop();
    }
    
    shutdown();
    return EXIT_SUCCESS;
}
