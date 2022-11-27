#pragma once
#include "vec3.h"
#include "Scene.h"
#include "Camera.h"
#include "Plane.h"

#define MAX_REBOUND 5//used both as max reflection rebound aswell as max light cast depending on reflectivness
#define SOFT_SHADOW_RAY_COUNT 20//WILL BE USED AS SQUARED VALUE !

#ifndef SPHERE
#define SPHERE
    typedef struct Sphere
    {
        float center[3];
        float radius;
        float reflectivness;
        unsigned char color[3];
    } Sphere;
#endif

#ifndef LIGHT
#define LIGHT
    typedef struct Light
    {
        Vec3 position, color;
        float radius;
    } Light;
#endif

#ifndef SCENE
#define SCENE
typedef struct Scene
{
    unsigned short sphere_count, light_count;
    Sphere * spheres;
    Light * lights;
    Camera * camera;
    Vec3 * background;
} Scene;
#endif

typedef struct Ray
{
    float shortest_distance;
    float origin[3];
    Vec3 * direction;
} Ray;

void Ray_set(float origin[3], Vec3 * direction, Ray * output);

//try intersect with given objects and place color value in given pixel (unsigned char[3])
unsigned char Ray_castSphere(Ray * ray, Sphere * sphere, Scene * scene, unsigned char output[3], unsigned char i );
unsigned char Ray_castPlane(Ray * ray, Plane * plane, Scene * scene, unsigned char output[3], unsigned char i );