#include "Scene.h"

//SPHERE
void Sphere_set(float * center, float radius, float reflectivness, unsigned char * color, Sphere * output)
{
    output->center[0] = center[0];
    output->center[1] = center[1];
    output->center[2] = center[2];

    output->color[0] = color[0];
    output->color[1] = color[1];
    output->color[2] = color[2];

    output->reflectivness = reflectivness;
    output->radius = radius;
}

//LIGHT
void Light_set(Vec3 position, Vec3 color, float radius, Light * output)
{
    Vec3_copy(&color, &(output->color) );
    Vec3_copy(&position, &(output->position) );

    float tmp = output->position.x;
    output->position.x = -output->position.y;
    output->position.y = -tmp;

    output->radius = radius;
}

//SCENE
void Scene_set(Camera * camera, Sphere * spheres, Light * lights, Vec3 * background, Scene * output)
{
    output->camera = camera;
    output->lights = lights;
    output->spheres = spheres;
    output->background = background;
}

void Scene_add_Light(Light * light, Scene * output)
{
    output->lights = (Light *) realloc( output->lights , output->sphere_count * sizeof( Light ) + sizeof( Light ) );
    memcpy( output->lights + output->sphere_count * sizeof( Light ) / sizeof( Light ) - 1, light, sizeof( Light ) );
}

void Scene_remove_Light(unsigned long id, Scene * output)
{
    if ( id > output->sphere_count * sizeof( Light ) /sizeof( Light ) )
    {
        printf("Try to access index out of range in Scene_remove_Light() !\n");
        return;
    }

    Light * lights;
    lights = (Light *) malloc( output->sphere_count * sizeof( Light ) - sizeof( Light ) );
    memcpy( lights, output->lights, id * sizeof( Light ) ); 
    memcpy( lights + sizeof(Light) * id, output->lights + sizeof(Light) * (id + 1), output->sphere_count * sizeof( Light ) - (id+1) * sizeof(Light) );
    free(output->lights);
    output->lights = lights;
}

void Scene_add_Sphere(Sphere * sphere, Scene * output)
{
    output->spheres = (Sphere *) realloc( output->spheres , output->sphere_count * sizeof( Sphere ) + sizeof( Sphere ) );
    memcpy( output->spheres + output->sphere_count * sizeof( Sphere ) - 1, sphere, sizeof( Sphere ) );
}

void Scene_remove_Sphere(unsigned long id, Scene * output)
{
    if ( id > output->sphere_count * sizeof( Sphere ) )
    {
        printf("Try to access index out of range in Scene_remove_Sphere() !\n");
        return;
    }

    Sphere * spheres;
    spheres = (Sphere *) malloc( output->sphere_count * sizeof( Sphere ) - sizeof( Sphere ) );
    memcpy( spheres, output->spheres, id * sizeof( Sphere ) ); 
    memcpy( spheres + sizeof(Sphere) * id, output->spheres + sizeof(Sphere) * (id + 1), output->sphere_count * sizeof( Sphere ) - (id+1) * sizeof(Sphere) );
    free(output->spheres);
    output->spheres = spheres;
}

void Scene_render(Scene * scene, Image * image)
{

    if (scene->camera->directionMatrix == NULL)
        Camera_genDirectionMatrix( scene->camera );

    for ( unsigned short x = 0; x < scene->camera->resolution[0]; x++ )
    {
        for ( unsigned short y = 0; y < scene->camera->resolution[1]; y++ )
        {

            unsigned char pixel[3] = {0, 0, 0};
            Vec3 pix;
            float tmp[3];

            memcpy(tmp, &scene->camera->directionMatrix[3 * ( y * scene->camera->resolution[0] + x)], sizeof( float ) * 3 );

            Vec3 dir; 
            Vec3_arrayToVec(tmp, &dir);

            Ray ray;
            Ray_set( scene->camera->position, &dir, &ray );

            for ( unsigned short i = 0; i < scene->sphere_count; i++ )
            {
                Ray_castSphere( &ray, &scene->spheres[i], scene, pixel, 0 );
            }

            pix.x = (float) pixel[0] / 255;
            pix.y = (float) pixel[1] / 255;
            pix.z = (float) pixel[2] / 255;

            Image_setPixel(image, scene->camera->resolution[1] - x - 1, y, &pix);
        }
    }

}