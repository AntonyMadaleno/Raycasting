#include "Sphere.h"

//SPHERE
void Sphere_set(Vec3 * center, float radius, float reflectivness, Vec3 * color, Sphere * output)
{
    output->center = center;
    output->color = color;
    output->reflectivness = reflectivness;
    output->radius = radius;
}