#ifndef _VECTOR_TYPES
#define _VECTOR_TYPES

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct
{
    float x;
    float y;
    float z;
} v3f;

inline void vectorCpyv(v3f *a, v3f *b)
{
    a->x = b->x;
    a->y = b->y;
    a->z = b->z;
}

inline void vectorCpys(v3f *v, float s)
{
    v->x = s;
    v->y = s;
    v->z = s;
}

inline void vectorAddv(v3f *a, v3f *b)
{
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

inline void vectorAdds(v3f *v, float s)
{
    v->x += s;
    v->y += s;
    v->z += s;
}

inline void vectorSubv(v3f *a, v3f *b)
{
    a->x -= b->x;
    a->y -= b->y;
    a->z -= b->z;
}

inline void vectorSubs(v3f *v, float s)
{
    v->x -= s;
    v->y -= s;
    v->z -= s;
}

inline void vectorMultv(v3f *a, v3f *b)
{
    a->x *= b->x;
    a->y *= b->y;
    a->z *= b->z;
}

inline void vectorMults(v3f *v, float s)
{
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

inline void vectorDivv(v3f *a, v3f *b)
{
    if (b->x != 0 && b->y != 0 && b->z != 0)
    {
        a->x /= b->x;
        a->y /= b->y;
        a->z /= b->z;
    }
}

inline void vectorDivs(v3f *v, float s)
{
    if (s != 0)
    {
        v->x /= s;
        v->y /= s;
        v->z /= s;
    }
}

inline void vectorAbs(v3f *v)
{
    v->x = fabs(v->x);
    v->y = fabs(v->y);
    v->z = fabs(v->z);
}

inline void vectorSigns(v3f *v)
{
    if (v->x != 0)
    {
        v->x = v->x / fabs(v->x);
    }
    if (v->y != 0)
    {
        v->y = v->y / fabs(v->y);
    }
    if (v->z != 0)
    {
        v->z = v->z / fabs(v->z);
    }
}

inline void vectorDist(float *d, v3f *v)
{
    *d = sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

inline void vectorNormalize(v3f *v)
{
    float d;
    vectorDist(&d, v);
    if (d != 0)
    {
        v->x = v->x / d;
        v->y = v->y / d;
        v->z = v->z / d;
    }
}
inline void vectorRandv(v3f *v, v3f *l)
{
    v->x = (rand() % (int)(l->x * 2.0)) - l->x;
    v->y = (rand() % (int)(l->y * 2.0)) - l->y;
    v->z = (rand() % (int)(l->z * 2.0)) - l->z;
}
#endif
