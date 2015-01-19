#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <list>

#ifndef _SIMULATION_SYSTEM
#define _SIMULATION_SYSTEM

#include "vector_types.h"

#define _SIM_BODY_MODE_CALM 0
#define _SIM_BODY_MODE_PANIC 1


typedef struct
{
    unsigned int id;
    int mode;
    int type;
    float speed;
    float weight;
    v3f swimVelocity;
    v3f position;
    v3f velocity;
    v3f minSpeed;
    v3f maxSpeed;
    v3f acceleration;
    int oob;
    float theta;
    float phi;
    float scale;
} Body;

typedef struct
{
    unsigned int id;
    int type;
    v3f position;
} Particle;

typedef struct
{
    // Body Simulation Factors
    float speed;
    float noise;
    float range;
    
    // Body accumulators
    int bodyCountID;
    int bodyCount;
    int bodyCountInit;
    int bodyCountMax;
    
    // Particle accumulators
    int particleCount;
    int particleCountInit;
    int particleCountMax;
    
    // Simulation Coordinates
    v3f origin;
    v3f bounds;
    
    // Containers
    std::list<Body> bodyContainer;
    std::list<Particle> particleContainer;

} SimulationSystem;


void stepSimulation(SimulationSystem *sim, float delta);
// Configures simulation system
int configureSimulation(SimulationSystem *sim, const char* configFile);

// Initializes simulation state
int initializeSimulation(SimulationSystem *sim);

// Spawns a body, returns body id
int spawnBody(SimulationSystem *sim);

// Spawns particle, returns particle id
int spawnParticle(SimulationSystem *sim);

// Step system with time delta

// Base Body Simulation calls
void bodySimulation(SimulationSystem *sim, float delta);
// Flocking rules
void bodySeparation(v3f* v, Body *body, std::list<Body> *container, float range, float delta);
void bodyAlignment(v3f* v, Body *body, std::list<Body> *container, int bodyCount, float range, float delta);
void bodyCohesion(v3f* v, Body *body, std::list<Body> *container, int bodyCount, float range, float delta);
void bodyBoundaries(v3f* v, v3f* bounds, v3f* origin, Body *body, float range, float delta);
void bodySpeedLimit(Body* body);
void bodyNoise(v3f* v, Body* body, float noise, float delta);

#endif
