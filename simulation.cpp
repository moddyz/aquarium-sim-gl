#include "simulation.h"

// Configures simulation system
int configureSimulation(SimulationSystem *sim, const char* configFile)
{
	if(FILE* fp = fopen(configFile, "r")){
		char line[128], attribute[128];
		int flag, category = 0;
		while(fgets(line, 128, fp)){
			sscanf(line, "%s", attribute);
			if      (!strcmp(attribute, "[simulation]"))     { category = 1; continue; }
			if      (!strcmp(attribute, "[body_type_0]"))     { category = 2; continue; }
			else if (!strcmp(line, "\n"))      		     { category = 0; continue; }
			
			switch(category){
				case 1: // Simulation Parameters
					if      (!strcmp(attribute, "speed"))  sscanf(line, "%*s = %f", &sim->speed);
					else if (!strcmp(attribute, "noise")) sscanf(line, "%*s = %f", &sim->noise);
					else if (!strcmp(attribute, "range")) sscanf(line, "%*s = %f", &sim->range);
					else if (!strcmp(attribute, "body_count_init")) sscanf(line, "%*s = %d", &sim->bodyCountInit);
					else if (!strcmp(attribute, "body_count_max")) sscanf(line, "%*s = %d", &sim->bodyCountMax);
					else if (!strcmp(attribute, "part_count_init")) sscanf(line, "%*s = %d", &sim->particleCountInit);
					else if (!strcmp(attribute, "part_count_max")) sscanf(line, "%*s = %d", &sim->particleCountMax);
					else if (!strcmp(attribute, "origin_x")) sscanf(line, "%*s = %f", &sim->origin.x);
					else if (!strcmp(attribute, "origin_y")) sscanf(line, "%*s = %f", &sim->origin.y);
					else if (!strcmp(attribute, "origin_z")) sscanf(line, "%*s = %f", &sim->origin.z);
					else if (!strcmp(attribute, "bounds_x")) sscanf(line, "%*s = %f", &sim->bounds.x);
					else if (!strcmp(attribute, "bounds_y")) sscanf(line, "%*s = %f", &sim->bounds.y);
					else if (!strcmp(attribute, "bounds_z")) sscanf(line, "%*s = %f", &sim->bounds.z);
					break;
				case 2: // Body Type 0 Parameters
					break;
			}
		}
		fclose(fp);
	}
	else{
		printf("Error: Unable to read \"%s\".\n", configFile);
		return -1;
	}

    printf("Simulation Configuration -- \n");
    printf("Speed -- %f \n", sim->speed);
    printf("Noise -- %f \n", sim->noise);
    printf("Range -- %f \n", sim->range);
    printf("Body Count Init -- %d \n", sim->bodyCountInit);
    printf("Body Count Max -- %d \n", sim->bodyCountMax);
    printf("Simulation Origin (%f, %f, %f) \n", sim->origin.x, sim->origin.y, sim->origin.z);    
    printf("Simulation Boundaries (%f, %f, %f) \n", sim->bounds.x, sim->bounds.y, sim->bounds.z);    

	return 0;
}

int initializeSimulation(SimulationSystem *sim)
{
    sim->bodyCount = 0;
    sim->bodyCountID = 0;
    sim->particleCount = 0;
    // Seed random
    srand(time(NULL));

    // spawn N bodies
    for (int b = 0; b < sim->bodyCountInit; ++b)
        spawnBody(sim);
}

// Spawns a body, returns body id
int spawnBody(SimulationSystem *sim)
{
    
    // Specification
    Body body;
    vectorCpys(&body.position, 0);
    vectorCpys(&body.velocity, 0);
    body.id = sim->bodyCountID++;
    body.mode = 0;
    body.oob = 0;
    body.type = (rand() % 9) + 2;
    body.speed = ((float)body.type);
    body.weight = body.speed * 0.1;
    
    // Random Position within bounds, plus center offset
    vectorRandv(&body.position, &(sim->bounds));
    vectorAddv(&body.position, &sim->origin);
    printf("Spawn Body Pos (%f, %f, %f)\n", body.position.x, body.position.y, body.position.z);
    
    body.maxSpeed.x = 10.0;
    body.maxSpeed.y = 4.0;
    body.maxSpeed.z = 10.0;
    vectorMults(&body.maxSpeed, sim->speed);
    vectorMults(&body.maxSpeed, body.speed);
    
    body.minSpeed.x = 3.0;
    body.minSpeed.y = 1.0;
    body.minSpeed.z = 3.0;
    vectorMults(&body.minSpeed, sim->speed);
    vectorMults(&body.minSpeed, body.speed);

    // Random Velocity
    vectorRandv(&body.velocity, &body.maxSpeed);
    bodySpeedLimit(&body);

    body.scale = body.weight; 
    
    sim->bodyContainer.push_back(body);

    sim->bodyCount++;

    return body.id;
}

// Step system with time delta
void stepSimulation(SimulationSystem *sim, float delta)
{   
    bodySimulation(sim, delta);               
}

// Flocking rules
void bodySimulation(SimulationSystem *sim, float delta)
{
    v3f s, a, c, b, n, pd;
    vectorCpys(&s, 0); vectorCpys(&a, 0); vectorCpys(&c, 0); vectorCpys(&b, 0);
    vectorCpys(&pd, 0);
    
    std::list<Body>::iterator bit = sim->bodyContainer.begin();
    // For each body
    for (; bit != sim->bodyContainer.end(); bit++)
    {
        bodySeparation(&s, &(*bit), &sim->bodyContainer, sim->range, delta);
        bodyAlignment(&a, &(*bit), &sim->bodyContainer, sim->bodyCount, sim->range, delta);
        bodyCohesion(&c, &(*bit), &sim->bodyContainer, sim->bodyCount, sim->range, delta);
        
        vectorDivs(&s, 0.5);
        vectorDivs(&a, 15.5);
        vectorDivs(&c, 18.5);
        
        vectorAddv(&(*bit).velocity, &s);
        vectorAddv(&(*bit).velocity, &a);
        vectorAddv(&(*bit).velocity, &c);
        
        bodySpeedLimit(&(*bit));

        // Calculate swim velocity
        
        bodyBoundaries(&b, &sim->bounds, &sim->origin, &(*bit), sim->range, delta);
        vectorDivs(&b, 0.5);
        vectorAddv(&(*bit).velocity, &b);
        
        bodyNoise(&n, &(*bit), sim->noise, delta * 100);

        n.y = bit->velocity.y;
        vectorNormalize(&n);
        //vectorDist(&(*bit).velocity);
        vectorAddv(&(*bit).velocity, &n);

        vectorCpyv(&pd, &(bit->velocity));
        vectorMults(&pd, delta);
        vectorAddv(&(bit->position), &pd);
        //printf("Body Velocity (%f, %f, %f)\n", (*bit).velocity.x, (*bit).velocity.y, (*bit).velocity.z);
        //printf("Body Position (%f, %f, %f)\n", (*bit).position.x, (*bit).position.y, (*bit).position.z);
    }
}

void bodySeparation(v3f* v, Body *body, std::list<Body> *container, float range,  float delta)
{
    v3f s;
    float d;
    
    std::list<Body>::iterator bit = container->begin();
    // For each body
    for (; bit != container->end(); bit++)
    {
        // If not subject body
        if (bit->id != body->id)
        {
            // Get distance between bodies
            vectorCpyv(&s, &body->position);
            vectorSubv(&s, &bit->position);
            vectorDist(&d, &s);
            
            if (d < range)
            {
                vectorSubv(v, &s);
            }
        }
    }

    vectorMults(v, delta);
}

void bodyNoise(v3f* v, Body* body, float noise, float delta)
{
    if (body->oob == 0)
    {
        vectorCpys(v, 0);
        v->x = sin(delta);
        v->z = cos(delta);
    }
}

void bodyAlignment(v3f* v, Body *body, std::list<Body> *container, int bodyCount, float range, float delta)
{
    v3f s;
    float d;

    int in_range = 0;
    
    std::list<Body>::iterator bit = container->begin();
    // For each body
    for (; bit != container->end(); bit++)
    {
        // If not subject body
        if (bit->id != body->id)
        {
            // Get distance between bodies
            vectorCpyv(&s, &body->position);
            vectorSubv(&s, &bit->position);
            vectorDist(&d, &s);
            
            if (d < range * 1)
            {
                vectorAddv(v, &bit->velocity);
                in_range++;
            }
        }
    }

    //printf("Alignment In Range: %d\n", in_range);
    vectorDivs(v, (float)(in_range));
    vectorMults(v, delta);
}


void bodyCohesion(v3f* v, Body *body, std::list<Body> *container, int bodyCount, float range, float delta)
{
    v3f s;
    float d;
    int in_range = 0;
    
    std::list<Body>::iterator bit = container->begin();
    // For each body
    for (; bit != container->end(); bit++)
    {
        if (bit->id != body->id)
        {
            vectorCpyv(&s, &body->position);
            vectorSubv(&s, &bit->position);
            vectorDist(&d, &s);
            
            if (d < range * 1)
            {
                vectorAddv(v, &bit->position);
                in_range++;
            }
        }
    }
    vectorDivs(v, (float)(in_range));
    vectorSubv(v, &body->velocity);
    vectorMults(v, delta);
}

void bodyGroupLimit(v3f* v, Body *body, std::list<Body> *container, int bodyCount, float range, float delta)
{
    v3f s;
    float d;
    int in_range = 0;
    
    std::list<Body>::iterator bit = container->begin();
    // For each body
    for (; bit != container->end(); bit++)
    {
        if (bit->id != body->id)
        {
            vectorCpyv(&s, &body->position);
            vectorSubv(&s, &bit->position);
            vectorDist(&d, &s);
            
            if (d < range * 1)
            {
                vectorAddv(v, &bit->position);
                in_range++;
            }
        }
    }
    //printf("Cohesion In Range: %d\n", in_range);
    vectorDivs(v, (float)(in_range));
    vectorSubv(v, &body->velocity);
    vectorMults(v, delta / 8);
}

void bodyBoundaries(v3f* v, v3f* bounds, v3f* origin, Body *body, float range, float delta)
{
    float xd = 0, yd = 0, zd = 0;
    vectorCpys(v, 0);   
    int oob = 0;
    
    v3f p_bounds, n_bounds;

    vectorCpyv(&p_bounds, bounds);
    vectorAddv(&p_bounds, origin); 

    vectorCpyv(&n_bounds, bounds);
    vectorMults(&n_bounds, -1.0);
    vectorAddv(&n_bounds, origin);

    float scaleFactor = 0.501;
    
    if (body->position.x > (p_bounds.x - range))
    {
        xd = p_bounds.x - body->position.x;
        if (xd <= 0)
            xd = 0.01;
        xd = range / xd;
        xd = fabs(xd);
        v->x = v->x + (xd *-1 * delta * scaleFactor);
        oob = 1;
    }
    if (body->position.y > (p_bounds.y - range))
    {
        yd = p_bounds.y - body->position.y;
        if (yd <= 0)
            yd = 0.01;
        yd = range / yd;
        yd = fabs(yd);
        v->y = v->y + (yd * -1 * delta * scaleFactor);
        oob = 1;
    }
    if (body->position.z > (p_bounds.z - range))
    {
        yd = p_bounds.z - body->position.z;
        if (zd <= 0)
            zd = 0.01;
        zd = range / zd;
        zd = fabs(zd);
        v->z = v->z + (-1 * zd * delta * scaleFactor);
        oob = 1;
    }
    if (body->position.x < (n_bounds.x + range))
    {
        xd = p_bounds.x - body->position.x;
        if (xd >= 0)
            xd = 0.01;
        xd = range / xd;
        xd = fabs(xd);
        v->x = v->x + (xd * delta * scaleFactor);
        oob = 1;
    }
    if (body->position.y < (n_bounds.y + range))
    {
        yd = p_bounds.y - body->position.y;
        if (yd >= 0)
            yd = 0.01;
        yd = range / yd;
        yd = fabs(yd);
        v->y = v->y + (yd * delta * scaleFactor);
        oob = 1;
    }
    if (body->position.z < (n_bounds.z + range))
    {
        yd = p_bounds.z - body->position.z;
        if (zd >= 0)
            zd = 0.01;
        zd = range / zd;
        zd = fabs(zd);
        v->z = v->z + (zd * delta * scaleFactor);
        oob = 1;
    }
    body->oob = oob;
}

void bodySpeedLimit(Body* body)
{
    v3f signs;
    vectorCpyv(&signs, &body->velocity);
    vectorSigns(&signs);
    
    if (body->oob == 0)
    {
        // Min
        if (fabs(body->velocity.x) < body->minSpeed.x)
           body->velocity.x = body->minSpeed.x * signs.x;
        if (fabs(body->velocity.y) < body->minSpeed.y)
           body->velocity.y = body->minSpeed.y * signs.y;
        if (fabs(body->velocity.z) < body->minSpeed.z)
           body->velocity.z = body->minSpeed.z * signs.z;
    }

    // Max
    if (fabs(body->velocity.x) > body->maxSpeed.x)
       body->velocity.x = body->maxSpeed.x * signs.x;
    if (fabs(body->velocity.y) > body->maxSpeed.y)
       body->velocity.y = body->maxSpeed.y * signs.y;
    if (fabs(body->velocity.z) > body->maxSpeed.z)
       body->velocity.z = body->maxSpeed.z * signs.z;

}
