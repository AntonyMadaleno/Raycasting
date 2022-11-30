#pragma once

#include "Vec3.h"

#ifndef SPHERE
#define SPHERE
    typedef struct Sphere
    {
        Vec3 * center;
        float radius;
        float reflectivness;
        Vec3 * color;
    } Sphere;
#endif

//SPHERE
void Sphere_set(Vec3 * center, float radius, float reflectivness, Vec3 * color, Sphere * output);